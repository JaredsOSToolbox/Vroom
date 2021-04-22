#include "../includes/tlb_t.hpp"
#include <tuple>

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
