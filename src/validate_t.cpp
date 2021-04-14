#include <tuple>
#include <stdint.h>
#include "../includes/validate_t.hpp"

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

std::tuple<uint32_t, uint32_t, uint32_t> validate_t::get_values() {
    return std::make_tuple(this->virt, this->physical, this->value);
    // usage outside this function
    // auto [virt, physical, value] = validate.get_values();
}

// FIXME : account for more types
//bool validate_t::operator==(const example_t& example){
  //return (
    //this->virt == example.a &&
    //this->physical == example.b &&
    //this->value == example.c
  //);
//}
