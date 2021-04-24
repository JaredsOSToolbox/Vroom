#include "../includes/tlb_t.hpp"
#include <tuple>
#include <iostream>
#include <iomanip>

tlb_t::tlb_t() {} // let the magic happen

bool tlb_t::is_full(){ return this->cache.size() == TLB_T_SIZE_LIMIT; }

void tlb_t::set_dirty_bit(size_t pos) {
  const auto[bit, address] = cache[pos];
  this->cache.insert(this->cache.begin() + pos, std::make_tuple(true, address));
}

address_t tlb_t::operator[](size_t index) const {
  const auto[bit, address] = cache[index];
  //this->set_dirty_bit(index); FIXME | is this going to be a std::weak_ptr situation where it doesn't affect the bit?
  return address;
}

address_t& tlb_t::operator[](size_t index) {
  auto[bit, address] = cache[index];
  this->set_dirty_bit(index);
  return address;
}

void tlb_t::replace_line(size_t index, address_t content){
  this->cache.insert(this->cache.begin() + index, std::make_tuple(false, content));
}

address_t* tlb_t::query_table(address_t value) {
  for(auto element : this->cache){
    auto[tag, add] = element;
    if(add == value){ return &add; }
  }
  return nullptr;
}
std::vector<std::tuple<bool, address_t>> tlb_t::get_cache() const { return this->cache; }

std::ostream& operator<<(std::ostream& os, const tlb_t& t) {
  std::vector<std::tuple<bool, address_t>> _cache = t.get_cache();
  os << "Bit | " << std::setw(5) << "Page Number |" << std::setw(5) << " Frame Number |"  <<  std::setw(8) << " Value"<< std::endl;
  os << std::string(50, '-') << std::endl;
  for(size_t i = 0; i < _cache.size(); ++i) {
    const auto[bit, address] = _cache[i];
    os << std::setw(2) << bit << std::setw(10) << address.get_page_number() << std::setw(12) << address.get_frame() << std::setw(16) << address.get_logical_address() << std::endl;
  }
  return os;
}
