#include "../includes/tlb_t.hpp"
#include "../includes/time_f.hpp"
#include <queue>
#include <iostream>
#include <iomanip>

tlb_t::tlb_t() {
  for(size_t i = 0; i < TLB_T_SIZE_LIMIT; ++i) {
    this->available_slots.push(i);
  }

  this->cache.reserve(PAGE_TABLE_SIZE);
  this->size_ = 0;
} // let the magic happen

bool tlb_t::is_full(){ return this->size_ == TLB_T_SIZE_LIMIT; }

entry::entry_t<address_t, signed char*>* tlb_t::operator[](size_t index) const {
  // NOTE
  // If an element in the cache decays to a nullptr, we should give back
  // the index it had in the table
  entry::entry_t<address_t, signed char*>* obj = this->cache[index];
  obj->bit = 1; // set bit to indiciate that we are using this entry
  return obj;
}

void tlb_t::insert(size_t index, entry::entry_t<address_t, signed char*>* content){
  this->cache.push_back(content);
  //this->cache.insert(this->cache.begin() + index, content);
  this->size_++;
}

void tlb_t::prune_cache() {
  std::vector<std::vector<entry::entry_t<address_t, signed char*>*>::iterator>
      container;  // NOTE, please tell me how I can make this better
  // FIXME : removing first element (FIFO, lazy implementation)
  this->cache.pop_back();
  this->available_slots.push((size_t)TLB_T_SIZE_LIMIT);
}

entry::entry_t<address_t, signed char*>* tlb_t::query_table(unsigned page_number) {
  for(auto element : this->cache) {
    auto x = element->data.get_page_number();
    if(x == page_number){ return element; }
  }
  return nullptr; // if not found , return nothing
}
std::vector<entry::entry_t<address_t, signed char*>*> tlb_t::get_cache() const { return this->cache; }

size_t tlb_t::slot_available() {
  if(this->size_ == 0) { return 0; }

  if(this->size_ == TLB_T_SIZE_LIMIT || this->available_slots.empty()) {
    /*
     * we need to select a victim
    */
    this->prune_cache();
  } 

  size_t slot = this->available_slots.front();
  this->available_slots.pop();
  return slot;
}

size_t tlb_t::size() { return this->cache.size(); }

void tlb_t::clear() {
  this->cache.clear();
  for(size_t i = 0; i < TLB_T_SIZE_LIMIT; ++i) {
    this->available_slots.push(i);
  }
  this->size_ = 0;
}

std::ostream& operator<<(std::ostream& os, const tlb_t& t) {
  //std::vector<std::tuple<bool, address_t>> _cache = t.get_cache();
  //os << "Bit | " << std::setw(5) << "Page Number |" << std::setw(5) << " Frame Number |"  <<  std::setw(8) << " Value"<< std::endl;
  //os << std::string(50, '-') << std::endl;
  //for(size_t i = 0; i < _cache.size(); ++i) {
    //const auto[bit, address] = _cache[i];
    //os << std::setw(2) << bit << std::setw(10) << address.get_page_number() << std::setw(12) << address.get_frame() << std::setw(16) << address.get_logical_address() << std::endl;
  //}
  return os;
}
