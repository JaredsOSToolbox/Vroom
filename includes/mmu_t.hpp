#pragma once

#include "address_t.hpp"
#include "page_table_t.hpp"
#include "reader_t.hpp"
#include "tlb_t.hpp"
#include "queue.hpp"

#include <string>
#define PHYSICAL_MEMORY_SIZE 128

class mmu_t {
  public:
    mmu_t(std::string, std::string, std::string, int);
    ~mmu_t();
    void conduct_test(int);
    void test_directly_to_memory();
    void set_frame(unsigned*);
    void consult_tlb(entry::entry_t<address_t, signed char*>*, unsigned*);
    void consult_page_table(unsigned, entry::entry_t<address_t, signed char*>*, unsigned*);
    void clear();

  private:

    address_reader_t add_reader;
    backing_store_reader_t backing_store;
    validate_reader_t correct;
    tlb_t translation_buffer; // DONE
    entry::page_table_t<address_t, signed char*> page_table; // DONE
    //signed char** physical_memory;
    circular::queue<int> frame_queue;
    unsigned tlb_hits, page_faults, page_table_hit, maximum_size;
};
