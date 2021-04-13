#include <iostream>
#include <vector>

#include "includes/address_t.hpp"
#include "includes/reader_t.hpp"


void test_without_reading(void){
  std::vector<uint32_t> container({1, 256, 32768, 128, 65534, 33153});
  for(uint32_t a : container){
    address_t addi = address_t(a);

    std::cout << "number: " << a << std::endl;
    std::cout << "page: " << addi.get_page_number() << std::endl;
    std::cout << "offset: " << addi.get_offset() << std::endl;
    std::cout << "------------" << std::endl;
  }
}

void test_with_reading(void) {
  reader_t reader = reader_t("inputs/addresses_tiny.txt");

  for(auto element : reader.get_parsed_contents()){
    std::cout << "number: " << element.get_value() << std::endl; 
    std::cout << "page: " << element.get_page_number() << std::endl; 
    std::cout << "offset: " << element.get_offset() << std::endl; 
    std::cout << "------------" << std::endl;
  }
}


int main(void) {

  return 0;
}
