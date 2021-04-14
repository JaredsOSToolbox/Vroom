#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

#include "includes/address_t.hpp"
#include "includes/reader_t.hpp"
#include "includes/validate_t.hpp"


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

void test_with_reading(void) {
  address_reader_t ad_reader = address_reader_t("inputs/addresses_small.txt");
  ad_reader.produce_parsed_contents();
  validate_reader_t val_reader = validate_reader_t("inputs/correct_small.txt");
  val_reader.process_content();

  assert(val_reader == ad_reader);

  //if(val_reader == ad_reader){
    //std::cout << "this worked" << std::endl;
  //}
  //for(auto element : ad_reader.get_content()) {
    //printf("logical: %5u (page: %3u, offset: %3u) --> physical %5u -- %s\n",
           //element.get_logical_address(), element.get_page_number(), element.get_offset(),
           //element.get_physical_address(), (true) ? "passed" : "failed");
  //}
}


int main(void) {
  test_with_reading();
  return 0;
}
