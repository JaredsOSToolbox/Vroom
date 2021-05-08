#include "../includes/address_t.hpp"

#include <vector>
#include <stdint.h>
#include <cmath>
#include <limits>

address_t::address_t(uint32_t logical_address){
  this->logical_address = logical_address;
  this->page_number = (unsigned)((logical_address & 0xFF00) >> 8);
  this->offset = (unsigned)(logical_address & 0xFF);
  // FIXME
  this->frame = std::numeric_limits<size_t>::infinity();
  this->physical_address = this->calculate_physical_address();
}

address_t::address_t(){
  this->offset = this->page_number = this->logical_address = (uint32_t)0;
}

uint32_t address_t::get_logical_address() const { return this->logical_address; }

unsigned address_t::get_offset() const { return this->offset; }

unsigned address_t::get_page_number() const { return this->page_number; }

unsigned address_t::get_physical_address() const { return this->physical_address; }

int address_t::get_value() const { return this->value; }
unsigned address_t::get_frame() const { return this->frame; }
void address_t::assign_frame(int value){ this->frame = value; }
void address_t::assign_value(int value){ this->value = value; }

unsigned address_t::calculate_physical_address() const {
  // FIXME
  if(this->frame == EOF){ return std::numeric_limits<unsigned>::infinity(); }
  return (this->frame << 8) | this->offset;
  //return this->frame * FRAME_SIZE  + this->offset;
}

std::ostream& operator<<(std::ostream& os, const address_t& address) {
  char buf[4096];
  std::sprintf(buf, "logical: %5u (page: %3u, offset: %3u) --> physical %5u --",
               address.get_logical_address(), address.get_page_number(),
               address.get_offset(), address.calculate_physical_address());
  os << buf;
  return os;
}

bool address_t::operator==(const address_t& value) const {
  return ((this->logical_address == value.get_logical_address()) &&
          (this->offset == value.get_offset()) &&
          (this->page_number == value.get_page_number()) &&
          //(this->physical_address == value.get_physical_address()) &&
          (this->frame == value.get_frame()));
}
