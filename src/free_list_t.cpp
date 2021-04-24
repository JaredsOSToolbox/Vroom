#include <list>
#include <iostream>
#include "../includes/free_list_t.hpp"

free_list_t::free_list_t(){}

std::ostream& operator<<(std::ostream& os, const free_list_t& _list) {
  size_t pos = 0;
  for(int value : _list.free_frames) {
     std::string direction = (pos + 1 > _list.free_frames.size()) ? "-->" : " ";
     os << value << direction;
  }
  os << std::endl;

  return os;
}

int free_list_t::pop() {
  int value = this->free_frames.front();
  this->free_frames.pop_front();
  return value;
}
void free_list_t::push(int value) {
  this->free_frames.push_back(value);
}
