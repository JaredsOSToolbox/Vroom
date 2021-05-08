#include <tuple>
#include <stdint.h>
#include "../includes/validate_t.hpp"
#include "../includes/address_t.hpp"

// This is not accessible throught the code base
// NOTE: MARKED FOR REMOVAL

//class example_t {
  //public:
    //example_t(int, int, int);
    //uint32_t a, b, c;
//};

//example_t::example_t(int x, int y, int z){
  //this->a = x;
  //this->b = y;
  //this->c = z;
//}

// --------------------------------------------

validate_t::validate_t(uint32_t v, uint32_t p, uint32_t value){
  this->virt = v;
  this->physical = p;
  this->value = value;
}
validate_t::validate_t(){
  this->virt = this->physical = this->value = 0;
}

std::tuple<uint32_t, uint32_t, uint32_t> validate_t::get_values() const {
    return std::make_tuple(this->virt, this->physical, this->value);
    // usage outside this function
    // auto [virt, physical, value] = validate.get_values();
}

uint32_t validate_t::get_physical_address() const { return this->physical; }
uint32_t validate_t::get_value() const { return this->value; }

bool validate_t::operator==(const address_t& other){
  //return true;
  return (
    //this->virt == other.get_logical_address() &&
    this->physical == other.get_physical_address() &&
    // TODO
    this->value == other.get_value()
  );
}

bool validate_t::operator==(const int& value) {
  return this->value == value;
}

bool validate_t::operator!=(const address_t& other){
  return ! this->operator==(other);
}

std::ostream& operator<<(std::ostream& os, const validate_t& va) {
  char buf[4096];
  const auto[l, p, v] = va.get_values();
  std::sprintf(buf, "logical: %5u --> physical: %5u --> value: %5u", l, p, v);
  os << buf;
  return os;
}
