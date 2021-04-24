#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

#include "includes/address_t.hpp"
#include "includes/reader_t.hpp"
#include "includes/validate_t.hpp"
#include "includes/tlb_t.hpp"


void test_without_reading(void){
  std::vector<uint32_t> container({1, 256, 32768, 128, 65534, 33153});
  int i = 0;
  for(uint32_t a : container){
    address_t addi = address_t(i++, a);

    std::cout << "number: " << a << std::endl;
    std::cout << "page: " << addi.get_page_number() << std::endl;
    std::cout << "offset: " << addi.get_offset() << std::endl;
    std::cout << "------------" << std::endl;
  }
}

void print_vector(std::vector<uint32_t> _buffer) {
  for(auto element : _buffer) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
}

void assertion_tests(void) {
  #define NUM 30198

  address_reader_t ad_reader = address_reader_t("inputs/addresses_tiny.txt");
  ad_reader.produce_parsed_contents();
  backing_store_reader_t back_reader = backing_store_reader_t("test/BACKING_STORE.bin");
  auto line = ad_reader[6];
  auto __offset = line.get_offset();
  auto __page = line.get_page_number();
  back_reader.seek_buffer(__page);

  std::cout << (int)back_reader[__offset] << std::endl;

  //for(int i = 0; i < FRAME_SIZE; ++i) {
    //std::cout << (int)back_reader[i] << " ";
  //}
  //std::cout << std::endl;

  //back_reader.process_content();
  validate_reader_t val_reader = validate_reader_t("inputs/correct_small.txt");
  val_reader.process_content();


  //assert(val_reader == ad_reader);
  
  /*
   * address_t basic operations
  */

  address_t add_a = address_t(uint32_t(NUM), 0);
  address_t add_b = address_t(uint32_t(NUM), 0);
  assert(uint32_t(NUM) == add_a.get_logical_address());
  assert(add_a == add_b);

  /*
   * TLB hit and miss
  */

  tlb_t _tldr = tlb_t();
  address_t* resultant = _tldr.query_table(add_a);
  assert(resultant == nullptr);
  _tldr.replace_line(0, add_a);
  assert(_tldr.query_table(add_a) != nullptr);
  assert((*_tldr.query_table(add_a)).get_logical_address() == uint32_t(NUM));

  /*
   * Check if address in file is inside the TLB directly
  */

  //auto element = *(_tldr.query_table(add_a));
  //auto from_ad_reader = ad_reader[6];

  ////std::cout << element  << std::endl;
  //std::cout << from_ad_reader << std::endl;
  //unsigned _frame = from_ad_reader.get_frame();
  //std::cout << "FRAME: " << _frame << std::endl;
  //uint32_t _offset = from_ad_reader.get_offset();
  //std::cout << "OFFSET: " << _offset << std::endl;
  //auto line = back_reader[_frame];
  //print_vector(line);

  //assert(from_ad_reader == element);
  //
  //std::cout << back_reader << std::endl;
  //for(auto addi : ad_reader.get_content()) {
    //unsigned _frame = addi.get_frame();
    //std::cout << "FRAME: " << _frame << std::endl;
    //uint32_t _offset = addi.get_offset();
    //std::cout << "OFFSET: " << _offset << std::endl;
    //auto line = back_reader[_frame];

    //print_vector(line);

    //std::cout  << std::endl;
    //printf("back_reader[%d][%d]: %d\n", _frame, _offset, back_reader[_frame][_offset]);
  //}
  

  //std::cout << _offset/4 <<  " " << line[_offset/4] << std::endl;
  //std::cout << _offset/64 <<  " " << line[_offset/64] << std::endl;
  //std::cout << line[_offset/4] << std::endl;

  //std::cout << _frame << " " << _offset << std::endl;
  ////uint32_t value = back_reader[_frame][_offset];
  //uint32_t value = back_reader[_offset][0];
  //std::cout << value << std::endl;
  
  //assert(_tldr[0] == add_a);

  //backing_store_reader_t back_reader = backing_store_reader_t("test/BACKING_STORE.bin");
  //back_reader.process_content();
  //assert(line_0.size() == 64);
}

void test_with_reading(void) {

  address_reader_t ad_reader = address_reader_t("inputs/addresses_tiny.txt");
  ad_reader.produce_parsed_contents();
  auto element = ad_reader[2];
  std::cout << element.get_page_number() << std::endl;
  /*
   * Consult the TLB (cache)
  */

  //bool consult_tlb = tlb.search(element);
  //if(!consult_tlb) {
    /*
     * Consult RAM
    */

  //} else {
    //printf("[SUCCESS] TLB hit!\n");
  //}
  //for(auto element : ad_reader.get_content()) {
    //printf("logical: %5u (page: %3u, offset: %3u) --> physical %5u -- %s\n",
           //element.get_logical_address(), element.get_page_number(), element.get_offset(),
           //element.get_physical_address(), (true) ? "passed" : "failed");
  //}
}


int main(void) {
  /*
   * DEMAND PAGING WHEN STARTING
  */
  assertion_tests();
  return 0;
}
