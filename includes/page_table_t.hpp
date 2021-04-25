#pragma once

#include "reader.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <list>

#define MAX_REF_COUNT 100
#define FREE_LIST_SIZE 128
#define PAGE_TABLE_SIZE FREE_LIST_SIZE

#define _T template<typename T>

_T

struct entry_t {
  T data; // data used (in our case this will be addresses)
  uint8_t bit; // dirty bit
  int reference_count; // current reference count
  int maximum_referernces;  // set timer on the number of times an object can be
                            // referenced
  entry_t() {}
  entry_t(T _data) {
    data = _data;
    bit = 0; // set dirty bit to false
    maximum_referernces =  MAX_REF_COUNT;
  }
};

//typedef struct entry_t<T> entry_t<T>;

_T

class page_table_t {
  public:
    page_table_t() {
      this->entries = std::vector<struct entry_t<T>*>(PAGE_TABLE_SIZE);
      this->in_use = std::list<size_t>(FREE_LIST_SIZE);
    }

    struct entry_t<T>* operator[](size_t index) {
      struct entry_t<T>* obj = this->entries[index];

      if(obj->reference_count == 0 || obj->maximum_referernces == 1) {
        delete this->entries[index];
        for(auto it = this->in_use.begin(); it != this->in_use.end(); ++it) {
          if(*it == index) {
            this->in_use.erase(it);
            break;
          }
        }
        return nullptr;
      }

      obj->reference_count++;
      if(obj->reference_count >= obj->maximum_referernces) {
        obj->reference_count >>= 1;
        obj->maximum_referernces >>= 1;
      } 
      return obj;
    }

    void insert(struct entry_t<T>* entry, int position) {
      this->entries.insert(this->entries.begin() + position, entry);
      this->in_use.push_front(position);
    }

  private:
    std::vector<struct entry_t<T>*> entries;
    std::list<size_t> in_use;
}; 

int main(void) {
  page_table_t<uint32_t> t = page_table_t<uint32_t>();

  struct entry_t<uint32_t>* entity = new entry_t((uint32_t)100);

  t.insert(entity, 0);

  for(int i = 0; i < 107; ++i) {
    entry_t<uint32_t>* element = t[0];
    if(element != nullptr) {
        std::cout << element->reference_count  << std::endl;
        std::cout  << "max reference count is " << element->maximum_referernces << std::endl;
    } else {
      std::cout << "freeing from the free list" << std::endl;
      break;
    }
  }

  return 0;
}
