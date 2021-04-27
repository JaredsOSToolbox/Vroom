#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include <cassert>
#include <iostream>

mmu_t::mmu_t(std::string addresses, std::string backing,
    std::string validator) : backing_store(backing), page_table()  {
  this->add_reader = address_reader_t(addresses);
  this->correct =  validate_reader_t(validator);
  this->translation_buffer = tlb_t();
  this->physical_memory = new signed char*[FRAME_COUNT];
  for(int i = 0; i < FRAME_SIZE; ++i) {
    this->physical_memory[i] = new signed char[FRAME_COUNT];
  }
}

mmu_t::~mmu_t() {
  for(int i = 0; i < FRAME_COUNT; ++i) {
    delete [] this->physical_memory[i];
  }
  delete [] this->physical_memory;
}

void mmu_t::conduct_test() {
  this->add_reader.produce_parsed_contents();
  this->correct.process_content();

  bool page_table_condition = false;
  bool tlb_condition = false;
  // we should be nested in a for loop
  auto line = this->add_reader[2];
  std::cout << line << std::endl;
  /*
   * Check the TLB
  */



  /*
   * Check the Page Table
  */

  int counter = 0; // attempt to restart the request
  int max = 2; // n+1 iterations, where n is the max

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
      _retreived->bit = 1; // set the bit because the frame entry is now valid
    } else {
      /*
       * TLB miss, now consult the page_table
      */
      auto _page_table_query = this->page_table[__frame];
      if(_page_table_query == nullptr) {
        /*
         * Page table miss; PAGE FAULT
         * Go to physical 
        */
        backing_store.seek_buffer(__page);
        _entry->container = backing_store.get_buffer();
        this->page_table.insert(_entry, __frame);
        std::cout << "went to physical memory" << std::endl;
        //this->page_table
      } else {
        std::cout << "value we got from memory is: " << (int)_entry->container[__offset] << std::endl;
      }

    }

    

    //auto _retreived = this->page_table[__frame];
    //if(_retreived != nullptr) {
      //_retreived->bit = 1; // set the bit because the frame entry is now valid
      //auto _restultant = this->translation_buffer.query_table(_retreived);
      
      //// TLB entry not found
      
      //if(_restultant == nullptr) {
        //auto next_slot = this->translation_buffer.slot_available();
        //this->translation_buffer.insert(next_slot, _retreived); // put content into the buffer
      //}
    //} else {
      /*
       * Missing entry
       * - emplace into the page table
      */

      //entry::entry_t<address_t, signed char*>* _entry = new entry::entry_t<address_t, signed char*>(line);
      //this->page_table.insert(_entry, __frame);
    //}

    page_table_condition = true;
    ++counter;
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
