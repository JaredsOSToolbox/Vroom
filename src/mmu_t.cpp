#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include "../includes/queue.hpp"

#include <cassert>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <bitset>

#define MAX_ITERATIONS 1000

#define TLB_HIT 0
#define PAGE_FAULT 1
#define CAN_BREAK 2

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

void mmu_t::consult_tlb(entry::entry_t<address_t, signed char*>* entry, unsigned* frame) {
  entry::entry_t<address_t, signed char*>* _retreived =
      this->translation_buffer.query_table(entry->data.get_page_number());

  if (_retreived != nullptr) {
    /*
     * TLB hit
     * In our case, we have also updated physical memory/the page table and we
     * can now get the proper value
     */
    _retreived->bit = 1;  // set the bit because the frame entry is now valid
    *frame = entry->data.get_frame();
    this->tlb_hits++;
    }
}

void mmu_t::consult_page_table(unsigned page_number, entry::entry_t<address_t, signed char*>* entry, unsigned* frame) {
  auto _page_table_query = this->page_table[page_number];

  if(_page_table_query == nullptr) {

    /*
     * Page table miss; PAGE FAULT
     * Go to physical 
     * This is not a problem when physical memory is the same size as the backing store
    */

    this->page_faults++;
  } 

  else {
    /*
     * We got an element from the page table
    */
    *frame = entry->data.get_frame();
  }

}

void mmu_t::conduct_test() {
  /*
   * Process file content
  */

  this->add_reader.produce_parsed_contents(); // FIXME
  this->correct.process_content();
  
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

  for(int i = 0; i < MAX_ITERATIONS; ++i) {
    address_t line = this->add_reader[i];
    auto _page = line.get_page_number();
    auto _offset = line.get_offset();

    auto frame_number = std::numeric_limits<unsigned>::infinity();

    entry::entry_t<address_t, signed char*>* _entry =
        new entry::entry_t<address_t, signed char*>(line);

    this->consult_tlb(_entry, &frame_number);
    if(is_inf(frame_number)) {
      this->consult_page_table(_page, _entry, &frame_number);
    } 

    if(is_inf(frame_number)) {
      this->set_frame(&line);
    }

    this->backing_store.seek_buffer(_page);
    _entry->container = this->backing_store.get_buffer();

    assert(_entry->container != nullptr);

    //if(this->translation_buffer.is_full()) {
      //std::cout  << "pruning is needed" << std::endl;
      //this->translation_buffer.prune_cache();
    //} else {

    //}
    size_t position = this->translation_buffer.slot_available();
    this->translation_buffer.insert(position, _entry);
    this->page_table.insert(_entry, _entry->data.get_frame());
    int value = _entry->container[_entry->data.get_offset()];
    assert(this->correct[i] == value);
  }

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;
  
  printf(
      "Access Count: %d\nTLB Hit Count: %d\nPage Fault Count: %d\nPage Table "
      "Hit: %d\n",
      MAX_ITERATIONS, this->tlb_hits, this->page_faults, this->page_table_hit);
  std::cout << "[INFO] Current size of the page table "
            << this->page_table.size() << std::endl;
  std::cout << "[INFO] Current size of the TLB is "
            << this->translation_buffer.size() << std::endl;
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
