#include "../includes/address_t.hpp"

#include <vector>
#include <stdint.h>
#include <cmath>

address_t::address_t(uint32_t value){
  this->value = value;
  this->page_number = (unsigned)((value & 0xFF00) >> 8);
  this->offset = (unsigned)(value & 0xFF);
}

address_t::address_t(){
  this->offset = this->page_number = this->value = (uint32_t)0;
}

uint32_t address_t::get_value() { return this->value; }

unsigned address_t::get_offset() { return this->offset; }

unsigned address_t::get_page_number() { return this->page_number; }
