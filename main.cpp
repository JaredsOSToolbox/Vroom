#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

#include "includes/address_t.hpp"
#include "includes/mmu_t.hpp"
#include "includes/reader_t.hpp"
#include "includes/tlb_t.hpp"
#include "includes/validate_t.hpp"
#include "includes/page_table_t.hpp"

#define ADDRESS_FILE "inputs/addresses_ALL.txt"
#define BACKING_STORE "inputs/BACKING_STORE.bin"
#define CORRECT_FILE "inputs/correct_ALL.txt"


//void test_without_reading(void){
  //std::vector<uint32_t> container({1, 256, 32768, 128, 65534, 33153});
  //int i = 0;
  //for(uint32_t a : container){
    //address_t addi = address_t(i++, a);

    //std::cout << "number: " << a << std::endl;
    //std::cout << "page: " << addi.get_page_number() << std::endl;
    //std::cout << "offset: " << addi.get_offset() << std::endl;
    //std::cout << "------------" << std::endl;
  //}
//}

void print_vector(std::vector<uint32_t> _buffer) {
  for(auto element : _buffer) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
}

void assertion_tests(void) {
  #define NUM 30198


  address_reader_t ad_reader = address_reader_t(ADDRESS_FILE);
  //ad_reader.produce_parsed_contents();
  backing_store_reader_t back_reader = backing_store_reader_t("inputs/BACKING_STORE.bin");

  validate_reader_t val_reader = validate_reader_t(CORRECT_FILE);
  val_reader.process_content();

  /*
   * Read the adresses and look them up in the backing store
   * Here is WORST CASE, as we are constantly referring to backing store
   * THRASHING
  */

  for(size_t i = 0; i < ad_reader.size(); ++i) {
  //for(size_t i = 0; i < 25; ++i) {
    auto line = ad_reader[i];
    auto __frame = line.get_frame();
    auto __offset = line.get_offset();
    auto __page = line.get_page_number();
    std::cout << "Frame " << __frame << std::endl;
    back_reader.seek_buffer(__page);
    int val = (int)back_reader[__offset];
    std::cout << val_reader[i] << " == " << val << std::endl;
    assert(val_reader[i] == val);
  }

  std::cout << "[INFO] All backing store lookups are correct" << std::endl;

  
  /*
   * address_t basic operations
  */

  address_t add_a = address_t(uint32_t(NUM));
  address_t add_b = address_t(uint32_t(NUM));
  assert(uint32_t(NUM) == add_a.get_logical_address());
  assert(add_a == add_b);
}

int main(void) {
  
  mmu_t mmu = mmu_t(ADDRESS_FILE, BACKING_STORE, CORRECT_FILE, 128);

  //mmu.conduct_test(200);
  mmu.test_directly_to_memory();
  
  //for(int i = 200; i <= 1000; i+=200) {
    //mmu.conduct_test(i);
  //}

  return 0;
}
