#pragma once

#include "reader_t.hpp"
#include "time_f.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <queue>

#define MAX_REF_COUNT 10
#define FREE_LIST_SIZE 128
#define PAGE_TABLE_SIZE 256
#define STALE_LIMIT 200
// ^ in milliseconds

#define FRAME_COUNT 256
#define FRAME_SIZE 256


#define THRASH_LIMIT 5
// ^ how many elements we're allowed to remove from the
// page table to prevent thrashing

#define _T template<typename T, typename K>
// Credits: eventually do this
// http://www.cplusplus.com/forum/beginner/54061/

namespace entry {

  _T
  /*
   * Instead of weird templating schemes,
   * we could have just used polymorphic inheritence
  */

  struct entry_t {
    T data; // data used (in our case this will be addresses)
    K container; // data being fed in from backing storea
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
    bool operator==(entry_t<T, K>* object) {
      //return (object == &this);
      return (object->data.get_frame() == this->data.get_frame());
    }

  };

  _T

  class page_table_t {
    public:
      page_table_t();
      ~page_table_t();
      struct entry_t<T, K>* operator[](size_t index);
      void insert(struct entry_t<T, K>* entry, size_t position);
      struct entry_t<T, K>* get_entry(size_t);
      void check_for_stale_entry();
      bool is_full();
      size_t available_position();
      size_t size();

      friend std::ostream& operator<<(std::ostream&, const page_table_t<T, K>&);

      void clear();

    private:
      std::vector<struct entry_t<T, K>*> entries;
      std::list<struct entry_t<T, K>*> in_use;
      std::queue<size_t> available_slots;
      time_f timer;
      size_t size_;
  }; 

};

#include "page_table_t.tcc"
