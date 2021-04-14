#include "../includes/address_t.hpp"

#include <vector>
#include <stdint.h>
#include <cmath>

address_t::address_t(uint32_t logical_address, int frame){
  this->logical_address = logical_address;
  this->page_number = (unsigned)((logical_address & 0xFF00) >> 8);
  this->offset = (unsigned)(logical_address & 0xFF);
  // FIXME
  this->physical_address = this->calculate_physical_address();
  this->frame = frame;
}

address_t::address_t(){
  this->offset = this->page_number = this->logical_address = (uint32_t)0;
}

uint32_t address_t::get_logical_address() { return this->logical_address; }

unsigned address_t::get_offset() { return this->offset; }

unsigned address_t::get_page_number() { return this->page_number; }

unsigned address_t::get_physical_address() { return this->physical_address; }

unsigned address_t::calculate_physical_address() {
  // FIXME
  return this->frame * FRAME_SIZE  + this->offset;
}
