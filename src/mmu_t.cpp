#include "../includes/mmu_t.hpp"
#include "../includes/reader_t.hpp"

#include <cassert>
#include <iostream>

mmu_t::mmu_t(std::string addresses, std::string backing,
    std::string validator) : backing_store(backing) {
  this->add_reader = address_reader_t(addresses);
  this->correct =  validate_reader_t(validator);
  this->translation_buffer = tlb_t();
}

void mmu_t::conduct_test() {
  this->add_reader.produce_parsed_contents();
  this->correct.process_content();

  for(size_t i = 0; i < this->add_reader.size(); ++i) {
    auto line = this->add_reader[i];
    auto __offset = line.get_offset();
    auto __page = line.get_page_number();
    this->backing_store.seek_buffer(__page);
    int val = (int)this->backing_store[__offset];
    assert(this->correct[i] == val);
  }

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;
}
