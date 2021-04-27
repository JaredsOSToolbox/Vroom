#pragma once

#include "address_t.hpp"
#include "page_table_t.hpp"
#include "reader_t.hpp"
#include "tlb_t.hpp"

#include <string>

class mmu_t {
  public:
    mmu_t(std::string, std::string, std::string);
    ~mmu_t();
    void conduct_test();

  private:
    address_reader_t add_reader;
    backing_store_reader_t backing_store;
    validate_reader_t correct;
    tlb_t translation_buffer;
    entry::page_table_t<address_t> page_table;
    signed char** physical_memory;
    //signed char physical_memory[FRAME_COUNT][FRAME_SIZE];
};
