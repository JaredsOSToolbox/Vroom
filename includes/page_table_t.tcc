#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include "time_f.hpp"

namespace entry {
  _T
  page_table_t<T, K>::page_table_t(){
      this->entries = std::vector<struct entry_t<T, K>*>(PAGE_TABLE_SIZE);
      this->in_use = std::list<struct entry_t<T, K>*>(FREE_LIST_SIZE);
      this->timer = time_f();
  } 
  _T
  page_table_t<T, K>::~page_table_t() {
    /*
     * We have to remvoe only one of the sources for the pointer
     * to avoid a double_free
   */
    for(auto element : this->entries) {
      delete element;
    }
  }
  _T

  struct entry_t<T, K>* page_table_t<T, K>::operator[](size_t index) {
    struct entry_t<T, K>* obj = this->entries[index];
    if(obj == nullptr){ return nullptr; }


    obj->reference_count++;
    obj->last_accessed = this->timer.get_time();

    if(obj->reference_count >= obj->maximum_referernces) {
      obj->reference_count >>= 1;
      obj->maximum_referernces >>= 1;
    } 

    if(obj->reference_count <= 0 || obj->maximum_referernces <= 0) {
      delete this->entries[index];
      for(auto it = this->in_use.begin(); it != this->in_use.end(); ++it) {
        if(*it == obj) {
          this->in_use.erase(it);
          break;
        }
      }
      return nullptr;
    }

    return obj;
  }
  _T

  void page_table_t<T, K>::insert(struct entry_t<T, K>* entry, size_t position) {
    this->entries.insert(this->entries.begin() + position, entry);
    this->in_use.push_front(entry);
  }
  _T

  struct entry_t<T, K>* page_table_t<T, K>::get_entry(size_t index) {
    /*
     * Use this method for checking the timestamp
     * Think of this as a weak pointer
    */

    return this->entries[index];
  }

  _T

  void page_table_t<T, K>::check_for_stale_entry() {
    /*
     * We need to prune entries in the use_list if they have
     * not been accessed in the last 200 milliseconds
    */

    long current_time = this->timer.get_time();

    std::vector<typename std::list<entry_t<T, K>*>::iterator> _remove_me;

    for(auto it = this->in_use.begin(); it != this->in_use.end(); ++it) {
      long duration_ = (current_time - (*it)->last_accessed);
      if(duration_ >= STALE_LIMIT) {
        _remove_me.push_back(
            it);  // we need to put them in a buffer to remove them after
                  // iterating over the list. There might be multiple matches
      }
    }

    for(auto garbage : _remove_me) { this->in_use.erase(garbage); }
  }

};
