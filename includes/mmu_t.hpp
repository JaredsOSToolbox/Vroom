#pragma once

#include "reader_t.hpp"
#include "tlb_t.hpp"

#include <string>

class mmu_t {
  public:
    mmu_t(std::string, std::string, std::string);
    void conduct_test();

  private:
    address_reader_t add_reader;
    backing_store_reader_t backing_store;
    validate_reader_t correct;
    tlb_t translation_buffer;
    // page_table_t page_table;
};
