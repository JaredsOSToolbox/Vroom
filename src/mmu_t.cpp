#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include "../includes/queue.hpp"

#include <cassert>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <bitset>

#define MAX_ITERATIONS 600

#define TLB_HIT 0
#define PAGE_FAULT 1

#define __T template <typename T>

__T

bool is_inf(T value){ return value == std::numeric_limits<T>::infinity(); }

mmu_t::mmu_t(std::string addresses, std::string backing,
    std::string validator) : backing_store(backing), page_table()  {
  
  this->add_reader = address_reader_t(addresses);
  this->correct =  validate_reader_t(validator);
  this->translation_buffer = tlb_t();
  this->physical_memory = new signed char*[PHYSICAL_MEMORY_SIZE];
  this->frame_queue = circular::queue<int>();

  for(int i = 0; i < PHYSICAL_MEMORY_SIZE; ++i) {
    this->physical_memory[i] = new signed char[FRAME_SIZE];
    this->frame_queue.push(i);
  }
  this->tlb_hits = 0;
  this->page_faults = 0;
  this->page_table_hit = 0;
}

mmu_t::~mmu_t(){}

void mmu_t::set_frame(address_t* line) {

  int assigned_frame_number;
  std::cout << this->page_table.size() << std::endl;
  
  if(line->get_frame() == EOF){
    /*
     * When we don't have an assigned frame, we will give them one
    */

    if(this->page_table.is_full()) {
      /*
       * We Need To Pick a Victim
      */
      this->page_table.check_for_stale_entry(); // FIXME : FIFO
      assigned_frame_number = this->page_table.available_position();
    } else {
      assigned_frame_number = this->frame_queue.pop();
    }

    line->assign_frame(assigned_frame_number);
  }
}

bool mmu_t::consult_tlb(entry::entry_t<address_t, signed char*>* entry, unsigned* frame) {
   /*
    * True : success, no need to check page table
    * False : failure, please consult the page table
  */
    entry::entry_t<address_t, signed char*>* _retreived = this->translation_buffer.query_table(entry);

    if(_retreived != nullptr) {
      /*
       * TLB hit
       * In our case, we have also updated physical memory/the page table and we can now get
       * the proper value
      */

      _retreived->bit = 1; // set the bit because the frame entry is now valid
      *frame = entry->data.get_frame();
      this->tlb_hits++;
      return true;
    }
    return false;
}

bool mmu_t::consult_page_table(unsigned page_number, entry::entry_t<address_t, signed char*>* entry, unsigned* frame) {
  /*
   * True : there was an element in the page table
   * False : there was NOT an element in the page table
  */

  auto _page_table_query = this->page_table[page_number];

  this->backing_store.seek_buffer(page_number);
  entry->container = this->backing_store.get_buffer();

  if(_page_table_query == nullptr) {

    /*
     * Page table miss; PAGE FAULT
     * Go to physical 
     * This is not a problem when physical memory is the same size as the backing store
    */

    this->page_faults++;
    size_t position = this->translation_buffer.slot_available();


    this->page_table.insert(entry, entry->data.get_frame());
    this->translation_buffer.insert(position, entry);
    return false;
  } 

  else {
    /*
     * We got an element from the page table
    */
    *frame = entry->data.get_frame();
    assert(entry->container != nullptr);
    return true;
  }

}

void mmu_t::conduct_test() {
  /*
   * Process file content
  */

  this->add_reader.produce_parsed_contents(); // FIXME
  this->correct.process_content();
  std::bitset<2> flags;
  /*
   * OVERVIEW
   *
   * (__offset)
   * (__page) -> TLB will yield -> (__frame)
   *
   * TLB Miss (__page) -> Page Table which will yield -> (__frame)
   * [__frame, __offset] -> Physical memory
   * We will in the end grab a value from ^ 
  */


  const int n = 2; // allows us to loop back on itself, restarting the query

  for(int i = 0; i < MAX_ITERATIONS; ++i) {
    bool cb = false; // Can break

    address_t line = this->add_reader[i];
    int value = std::numeric_limits<int>::infinity();

    if(line.get_frame() == EOF) {
      this->set_frame(&line);
    }



    int counter = 0; // attempt to restart the request
    int max = n; // n+1 iterations, where n is the max

    unsigned frame_from_hit = line.get_frame();

    entry::entry_t<address_t, signed char*>* _entry =
        new entry::entry_t<address_t, signed char*>(line);

    unsigned __page = line.get_page_number();

    /*
     * Check TLB
    */
    bool found_record = false;

    while(!cb || counter < max) {
      entry::entry_t<address_t, signed char*>* _retreived = this->translation_buffer.query_table(_entry);
      if(cb && !flags[PAGE_FAULT]){
        /*
         * we hop back to the top when a page fault occurs, we can give it to the page table to look up the entry
         * otherwise, we look up the entry in the TLB
        */
        found_record = this->consult_tlb(_retreived, &frame_from_hit);
        if(found_record){ flags.set(TLB_HIT, 1); }
      }

      if(!found_record) {
        /*
        * Check the Page Table
        */

        bool found_in_page_table = this->consult_page_table(__page, _entry, &frame_from_hit);
        if (!found_in_page_table) {
          flags.set(PAGE_FAULT, 1);
          cb = true;
        } 
        else {
          flags.set(PAGE_FAULT, 0);
        }
      }    
      ++counter;
    }
    counter = 0;
    if(!flags[PAGE_FAULT]){ this->page_table_hit++; }

    assert(frame_from_hit != std::numeric_limits<unsigned>::max());

    value = _entry->container[_entry->data.get_offset()];

    assert(this->correct[i] == value);

    std::cout << this->correct[i] << " == " << value << " (" << i << ") "
              << "ok" << ((flags[PAGE_FAULT]) ? " [PAGE FAULT] " : " [PAGE HIT] ")
              << ((flags[TLB_HIT]) ? " [TLB HIT] " : " [TLB MISS] ") << std::endl;
  }

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;
  
  printf(
      "Access Count: %d\nTLB Hit Count: %d\nPage Fault Count: %d\nPage Table "
      "Hit: %d\n",
      MAX_ITERATIONS, this->tlb_hits, this->page_faults, this->page_table_hit);
  std::cout << "[INFO] Current size of the page table "
            << this->page_table.size() << std::endl;
}

void mmu_t::test_directly_to_memory() {

  /*
   * All entries being checked
  */

  for(size_t i = 0; i < this->add_reader.size(); ++i) {
    auto line = this->add_reader[i];
    auto __offset = line.get_offset();
    auto __page = line.get_page_number();
    this->backing_store.seek_buffer(__page);
    int val = (int)this->backing_store[__offset];
    assert(this->correct[i] == val);
  }
  std::cout  << "[INFO] All assertions passed" << std::endl;
}
