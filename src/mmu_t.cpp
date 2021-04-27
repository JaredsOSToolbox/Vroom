#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include <cassert>
#include <iostream>

mmu_t::mmu_t(std::string addresses, std::string backing,
    std::string validator) : backing_store(backing), page_table()  {
  this->add_reader = address_reader_t(addresses);
  this->correct =  validate_reader_t(validator);
  this->translation_buffer = tlb_t();
}

void mmu_t::conduct_test() {
  this->add_reader.produce_parsed_contents();
  this->correct.process_content();

  bool page_table_condition = false;
  bool tlb_condition = false;
  // we should be nested in a for loop
  auto line = this->add_reader[0];
  /*
   * Check the TLB
  */



  /*
   * Check the Page Table
  */

  int counter = 0; // attempt to restart the request
  int max = 2; // n+1 iterations, where n is the max

  while(!page_table_condition || counter < max) {
    std::cout << "at the top of the loop" << std::endl;
    auto __offset = line.get_offset();
    auto __page = line.get_page_number();
    auto __frame = line.get_frame();
    

    auto _retreived = this->page_table[__frame];
    if(_retreived != nullptr) {
      _retreived->bit = 1; // set the bit because the frame entry is now valid
      auto _restultant = this->translation_buffer.query_table(_retreived);
      
      // TLB entry not found
      
      if(_restultant == nullptr) {
        auto next_slot = this->translation_buffer.slot_available();
        this->translation_buffer.insert(next_slot, _retreived); // put content into the buffer
      }
    } else {
      /*
       * Missing entry
       * - emplace into the page table
      */

      entry::entry_t<address_t>* _entry = new entry::entry_t<address_t>(line);
      this->page_table.insert(_entry, __frame);
    }

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
