#pragma once

#include "reader_t.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <list>

#define MAX_REF_COUNT 100
#define FREE_LIST_SIZE 128
#define PAGE_TABLE_SIZE FREE_LIST_SIZE

#define _T template<typename T>


namespace entry {

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
      reference_count = 0;
      maximum_referernces =  MAX_REF_COUNT;
    }
    bool operator==(entry_t<T>* object) {
      return (object == &this);
    }
  };

  _T

  class page_table_t {
    public:
      page_table_t();
      struct entry_t<T>* operator[](size_t index);
      void insert(struct entry_t<T>* entry, size_t position);
      //struct entry_t<T>* contains(struct entry_t<T>* entry);

    private:
      std::vector<struct entry_t<T>*> entries;
      std::list<struct entry_t<T>*> in_use;
  }; 

};

#include "page_table_t.tcc"
