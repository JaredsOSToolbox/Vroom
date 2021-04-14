#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <stdio.h>
#include <tuple>

/*
 * Reference
 *
 * https://github.com/JaredDyreson/Diablo/blob/master/lexical_analysis/src/lexer.cpp
*/

class example_t {
  public:
    example_t(int, int, int);
    uint32_t a, b, c;
};

example_t::example_t(int x, int y, int z){
  this->a = x;
  this->b = y;
  this->c = z;
}

// ------------------------------

class validate_t {
  public:
    validate_t();
    validate_t(uint32_t, uint32_t, uint32_t);

    std::tuple<uint32_t, uint32_t, uint32_t> get_values();
    bool operator==(const example_t&);
  private:
    uint32_t virt, physical, value;
};

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
}

bool validate_t::operator==(const example_t& example){
  return (
    this->virt == example.a &&
    this->physical == example.b &&
    this->value == example.c
  );
}


// --------------------------------------------------

uint32_t convert(std::string str) {
  return (uint32_t)std::strtol(str.c_str(), NULL, 10);
}
void test(void) {
  example_t b(16916, 20, 0);
  validate_t a;

  std::string line = "Virtual address: 16916 Physical address: 20 Value: 0";
  std::regex _re = std::regex(
      "Virtual address: ([0-9]+) Physical address: ([0-9]+) Value: ([0-9]+)");

  std::vector<uint32_t> container;

  for (auto it = std::sregex_iterator(line.begin(), line.end(), _re);
       it != std::sregex_iterator(); it++) {
    std::smatch sm = *it;
    a = validate_t(convert(sm.str(1)), convert(sm.str(2)), convert(sm.str(3)));

    }
  if(a == b){
    std::cout << "we parsed this correctly" << std::endl;
  }
}

  //printf("virt: %5u, phys: %5u, val: %5u\n", container[0], container[1],
         //container[2]);

int main(void) {
  test();
  return 0;
}
