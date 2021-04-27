#pragma once

#include "reader_t.hpp"
#include "time_f.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <list>

#define MAX_REF_COUNT 100
#define FREE_LIST_SIZE 128
#define PAGE_TABLE_SIZE FREE_LIST_SIZE
#define STALE_LIMIT 200
// ^ in milliseconds
#define FRAME_COUNT 256
#define FRAME_SIZE 256

#define _T template<typename T>


namespace entry {

  _T

  struct entry_t {
    T data; // data used (in our case this will be addresses)
    uint8_t bit; // dirty bit (0 means invalid and 1 means valid)
    int reference_count; // current reference count
    int maximum_referernces;  // set timer on the number of times an object can be
                              // referenced
    long last_accessed; // timestamp since it was last retrieved (for usage)

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
      ~page_table_t();
      struct entry_t<T>* operator[](size_t index);
      void insert(struct entry_t<T>* entry, size_t position);
      struct entry_t<T>* get_entry(size_t);
      void check_for_stale_entry();

    private:
      std::vector<struct entry_t<T>*> entries;
      std::list<struct entry_t<T>*> in_use;
      time_f timer;
  }; 

};

#include "page_table_t.tcc"
