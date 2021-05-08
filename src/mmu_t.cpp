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
    std::string validator, int physical_size) : backing_store(backing), page_table()  {
  
  this->add_reader = address_reader_t(addresses);
  this->correct =  validate_reader_t(validator);
  this->translation_buffer = tlb_t();
  //this->physical_memory = new signed char*[physical_size];
  this->frame_queue = circular::queue<int>();

  for(int i = 0; i < physical_size; ++i) {
    //this->physical_memory[i] = new signed char[FRAME_SIZE];
    this->frame_queue.push(i);
  }
  this->tlb_hits = 0;
  this->page_faults = 0;
  this->page_table_hit = 0;
  this->maximum_size = physical_size;
}

mmu_t::~mmu_t(){}

void mmu_t::set_frame(unsigned* frame) {

  
  if(is_inf(*frame)){
    /*
     * When we don't have an assigned frame, we will give them one
    */

    if(this->page_table.is_full()) {
      /*
       * We Need To Pick a Victim
      */
      this->page_table.check_for_stale_entry(); // FIXME : FIFO
      *frame = this->page_table.available_position();
    } 
    else {
      *frame = this->frame_queue.pop();
    }
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
    *frame = _page_table_query->data.get_frame();
  }

}

void mmu_t::conduct_test(int iterations) {
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


  for(int i = 0; i < iterations; ++i) {
    address_t line = this->add_reader[i];
    auto _page = line.get_page_number();
    std::bitset<2> flags;

    unsigned frame_number = std::numeric_limits<unsigned>::infinity();

    entry::entry_t<address_t, signed char*>* _entry =
        new entry::entry_t<address_t, signed char*>(line);

    this->consult_tlb(_entry, &frame_number);

    flags.set(TLB_HIT, (is_inf(frame_number)) ? 0 : 1);

    this->consult_page_table(_page, _entry, &frame_number);

    if(is_inf(frame_number)) {
      this->set_frame(&frame_number);
      flags.set(PAGE_FAULT, 1);
    } else {
      flags.set(PAGE_FAULT, 0);
    }

    if(flags[PAGE_FAULT]) {
      this->backing_store.seek_buffer(_page);
      _entry->container =  this->backing_store.get_buffer();
      this->page_table.insert(_entry, _entry->data.get_frame());
    } 

    assert(_entry->container != nullptr);

    size_t position = this->translation_buffer.slot_available();
    this->translation_buffer.insert(position, _entry);

    int value = _entry->container[_entry->data.get_offset()];
    //std::cout << this->correct[i] << " == " << value  << "  FRAME: " << frame_number << std::endl;
    assert(this->correct[i] == value);
  }

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;
  
  printf(
      "Access Count: %d\nTLB Hit Count: %d\nPage Fault Count: %d\n",
      iterations, this->tlb_hits, this->page_faults);
  this->clear();
}

void mmu_t::test_directly_to_memory() {

  /*
   * All entries being checked
  */
  this->add_reader.produce_parsed_contents(); // FIXME

  std::cout << this->add_reader.size() << std::endl;
  for(size_t i = 0; i < this->add_reader.size(); ++i) {
    auto line = this->add_reader[i];
    auto __offset = line.get_offset();
    auto __page = line.get_page_number();
    auto __frame = line.get_frame();
    this->backing_store.seek_buffer(__page);
    int val = (int)this->backing_store[__offset];
    line.assign_value(val);
    std::cout << line << std::endl;
    assert(
      (this->correct[i] == val) && 
      (this->correct[i].get_physical_address() == ((__frame << 8) | __offset))
    );
    //assert(this->correct[i] == line);
    //assert(value == val);
    //std::cout << value << std::endl;
  }
  std::cout  << "[INFO] All assertions passed" << std::endl;
}

void mmu_t::clear() {
  this->translation_buffer.clear();
  this->page_table.clear();
  for(int i = 0; i < this->maximum_size; ++i) {
    this->frame_queue.push(i);
  }
  this->tlb_hits = this->page_faults = this->page_table_hit = 0;
}
