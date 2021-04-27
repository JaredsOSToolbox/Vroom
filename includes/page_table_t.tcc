#include <iostream>
#include <memory>
#include <vector>
#include <list>

namespace entry {
  _T
  page_table_t<T>::page_table_t(){
      this->entries = std::vector<struct entry_t<T>*>(PAGE_TABLE_SIZE);
      this->in_use = std::list<struct entry_t<T>*>(FREE_LIST_SIZE);
  } 
  _T
  page_table_t<T>::~page_table_t() {
    /*
     * We have to remvoe only one of the sources for the pointer
     * to avoid a double_free
     * NOTE: may or may not break things
   */
    for(auto element : this->entries) {
      delete element;
    }
  }
  _T

  struct entry_t<T>* page_table_t<T>::operator[](size_t index) {
    if(this->entries[index] == nullptr){ return nullptr; }

    struct entry_t<T>* obj = this->entries[index];


    obj->reference_count++;
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

  void page_table_t<T>::insert(struct entry_t<T>* entry, size_t position) {
    this->entries.insert(this->entries.begin() + position, entry);
    this->in_use.push_front(entry);
  }

};
