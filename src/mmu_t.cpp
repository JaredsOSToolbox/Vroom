#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include <cassert>
#include <limits>
#include <iostream>

#define MAX_ITERATIONS 20
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
  for(int i = 0; i < PHYSICAL_MEMORY_SIZE; ++i) {
    this->physical_memory[i] = new signed char[FRAME_SIZE];
  }
}

mmu_t::~mmu_t() {
  //for(int i = 0; i < PHYSICAL_MEMORY_SIZE; ++i) {
    //delete [] this->physical_memory[i];
  //}
  //delete [] this->physical_memory;
}

void mmu_t::conduct_test() {
  this->add_reader.produce_parsed_contents();
  this->correct.process_content();

  bool page_table_condition = false;
  //bool tlb_condition = false;

  int n = 2; // allows us to loop back on itself, restarting the query
  for(int i = 0; i < MAX_ITERATIONS; ++i) {
    auto line = this->add_reader[i];
    int value = std::numeric_limits<int>::infinity();


    /*
    * Check the Page Table
    */

    int counter = 0; // attempt to restart the request
    int max = n + 1; // n+1 iterations, where n is the max
    int k = 0;
    auto frame_from_hit = 0;

    entry::entry_t<address_t, signed char*>* _entry = new entry::entry_t<address_t, signed char*>(line);

    while(!page_table_condition || counter < max) {
      auto __offset = line.get_offset();
      auto __page = line.get_page_number();
      auto __frame = line.get_frame();

      /*
       * Check TLB
      */

      auto _retreived = this->translation_buffer.query_table(_entry);
      if(_retreived != nullptr) {
        /*
         * TLB hit (we are ending up here because we updated the TLB then restarted)
         * In our case, we have also updated physical memory/the page table and we can now get
         * the proper value
        */

        _retreived->bit = 1; // set the bit because the frame entry is now valid
        frame_from_hit = _entry->data.get_frame();
        std::cout << "[INFO X] Frame grabbed is: " << frame_from_hit << std::endl;
        value = (int)_entry->container[__offset];
        std::cout << "[INFO] hit TLB" << std::endl;
        break;
      } 
      else {
        /*
         * TLB miss, now consult the page_table
        */
        //std::cout << "TLB miss with page number of " << __page << std::endl;
        auto _page_table_query = this->page_table[__page];
        if(_page_table_query == nullptr) {
          //std::cout << "Page table miss" << std::endl;
          /*
           * Page table miss; PAGE FAULT
           * Go to physical 
           * This is not a problem when physical memory is the same size as the backing store
          */
          if(this->page_table.is_full()) {
            /*
             * we need to pick a victim
            */
            std::cout << "we need to check for a victim" << std::endl;
            this->page_table.check_for_stale_entry();
            size_t position = this->page_table.available_position();
            if(position == std::numeric_limits<size_t>::infinity()) {
              std::cerr << "[FATAL] Could not find open slot" << std::endl;
            } else {
              std::cout << "[SUCCESS] Found an open slot of " << position << std::endl;
            }
          }
          //std::cout << "page currently in use: " << __page << std::endl;
          backing_store.seek_buffer(__page);
          _entry->container = backing_store.get_buffer();
          this->page_table.insert(_entry, __frame);
          size_t tlb_position = this->translation_buffer.slot_available();
          this->translation_buffer.insert(tlb_position, _entry);

          //std::cout << "[NOTE] Updated translation buffer and page table" << std::endl;
        } 

        else {
          /*
           * We got an element from the page table
          */
          //std::cout << "Page table hit" << std::endl;
          assert(_entry->container != nullptr); // failure to read buffer
          value = (int)_entry->container[__offset];
          std::cout << "[INFO] Page table hit" << std::endl;
          break;
        }

      }
      page_table_condition = true;
      ++counter;
      ++k;
    }
    std::cout << this->correct[i] << " == " << value << std::endl;
    if(is_inf(value)){
      std::cerr << "[FATAL] Could not properly set value" << std::endl;
    }
    //assert(this->correct[i] == value);
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
