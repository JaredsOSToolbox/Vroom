#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include "../includes/queue.hpp"

#include <cassert>
#include <limits>
#include <iostream>
#include <stdio.h>

#define MAX_ITERATIONS 30
#define RETURN_CONDITION 100

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
}

mmu_t::~mmu_t() {
  //for(int i = 0; i < PHYSICAL_MEMORY_SIZE; ++i) {
    //delete [] this->physical_memory[i];
  //}
  //delete [] this->physical_memory;
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

  bool cb = false; // Can break

  int n = 2; // allows us to loop back on itself, restarting the query

  for(int i = 0; i < this->add_reader.size(); ++i) {

    address_t line = this->add_reader[i];
    int value = std::numeric_limits<int>::infinity();

    int assigned_frame_number;
    
    if(line.get_frame() == EOF){
      /*
       * When we don't have an assigned frame, we will give them one
      */
      assigned_frame_number = this->frame_queue.pop();
      line.assign_frame(assigned_frame_number);
    }


    /*
    * Check the Page Table
    */

    int counter = 0; // attempt to restart the request
    int max = n + 1; // n+1 iterations, where n is the max
    int k = 0;
    unsigned frame_from_hit = std::numeric_limits<unsigned>::max();

    entry::entry_t<address_t, signed char*>* _entry =
        new entry::entry_t<address_t, signed char*>(line);

    while(!cb || counter < max) {
      unsigned __offset = line.get_offset();
      unsigned __page = line.get_page_number();
      unsigned __frame = line.get_frame(); 

      /*
       * Check TLB
      */

      entry::entry_t<address_t, signed char*>* _retreived = this->translation_buffer.query_table(_entry);
      if(_retreived != nullptr) {
        /*
         * TLB hit (we are ending up here because we updated the TLB then restarted)
         * In our case, we have also updated physical memory/the page table and we can now get
         * the proper value
        */

        _retreived->bit = 1; // set the bit because the frame entry is now valid
        frame_from_hit = _entry->data.get_frame();
        //value = (int)_entry->container[__offset];
        break;
      } 
      else {
        /*
         * TLB miss, now consult the page_table
        */
        auto _page_table_query = this->page_table[__page];
        if(_page_table_query == nullptr) {

          /*
           * Page table miss; PAGE FAULT
           * Go to physical 
           * This is not a problem when physical memory is the same size as the backing store
          */
          std::cout << "[INFO] Page Fault" << std::endl;
          size_t position;

          if(this->page_table.is_full()) {
            /*
             * we need to pick a victim
            */

            std::cout << "we need to check for a victim" << std::endl;
            this->page_table.check_for_stale_entry();
            position = this->page_table.available_position();

            if(position == std::numeric_limits<size_t>::infinity()) {
              std::cerr << "[FATAL] Could not find open slot" << std::endl;
            } else {
              std::cout << "[SUCCESS] Found an open slot of " << position << std::endl;
            }
          }

          backing_store.seek_buffer(__page);
          _entry->container = backing_store.get_buffer();

          this->page_table.insert(_entry, __frame);
          position = this->translation_buffer.slot_available();
          this->translation_buffer.insert(position, _entry);
        } 

        else {
          /*
           * We got an element from the page table
          */
          frame_from_hit = _entry->data.get_frame();

          backing_store.seek_buffer(__page);
          _entry->container = backing_store.get_buffer();

          assert(_entry->container != nullptr);

          value = (int)_entry->container[_entry->data.get_offset()];
          break;
        }

      }
      cb = true;
      ++counter;
      ++k;
    }

    assert(frame_from_hit != std::numeric_limits<unsigned>::max());

    value = _entry->container[_entry->data.get_offset()];

    std::cout << this->correct[i] << " == " << value << std::endl;
    assert(this->correct[i] == value);
  }

  /*
   * All entries being checked
  */

  //for(size_t i = 0; i < this->add_reader.size(); ++i) {
    //auto line = this->add_reader[i];
    //auto __offset = line.get_offset();
    //auto __page = line.get_page_number();
    //this->backing_store.seek_buffer(__page);
    //int val = (int)this->backing_store[__offset];
    //assert(this->correct[i] == val);
  //}

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;
}
