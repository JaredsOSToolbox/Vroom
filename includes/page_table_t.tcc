#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <limits>

#include "time_f.hpp"

namespace entry {
  _T
  page_table_t<T, K>::page_table_t(){
      this->entries = std::vector<struct entry_t<T, K>*>();
      this->entries.reserve(PAGE_TABLE_SIZE);
      this->in_use = std::list<struct entry_t<T, K>*>();
      //this->in_use.reserve(FREE_LIST_SIZE);
      this->timer = time_f();
      for (size_t i = 0; i < PAGE_TABLE_SIZE; ++i) {
        this->available_slots.push(i);
      }
  } 
  _T
  page_table_t<T, K>::~page_table_t() {
    /*
     * We have to remvoe only one of the sources for the pointer
     * to avoid a double_free
   */
    //for(auto element : this->entries) {
      //delete element;
    //}
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
      //delete this->entries[index]; FIXME
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
    this->entries.insert(this->entries.begin(), position, entry);
    //this->entries[position] = entry;
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

    //long current_time = this->timer.get_time();

    std::vector<typename std::list<entry_t<T, K>*>::iterator> _remove_me;
    size_t i = 0;
    for(auto it = this->in_use.begin(); it != this->in_use.end(); ++it) {
      if((*it)->reference_count < 2 ) {
        std::cout << "removing " << i << " from cache" << std::endl;
        _remove_me.push_back(it);
        this->available_slots.push(i);
      }
      //long duration_ = (current_time - (*it)->last_accessed);
      //if(duration_ >= STALE_LIMIT) { // NOTE: this is going to decay to FIFO
        //_remove_me.push_back(
            //it);  // we need to put them in a buffer to remove them after
                  //// iterating over the list. There might be multiple matches
        //this->available_slots.push(i);
      //}
      ++i;
    }

    for(auto garbage : _remove_me) { this->in_use.erase(garbage); }
  }

  _T
  bool page_table_t<T, K>::is_full() { return this->entries.size() == FRAME_COUNT; }
  _T
  size_t page_table_t<T, K>::available_position(){
    if(this->available_slots.empty()){
      return std::numeric_limits<size_t>::infinity(); // equivalent of not finding a spot
    }
    size_t position = this->available_slots.front();
    this->available_slots.pop();
    return position;
  }

  //_T
  //std::ostream& operator<<(std::ostream& os, const entry::page_table_t<T, K> table) {
    //os << "hello world" << std::endl;
    //return os;
  //}

};
