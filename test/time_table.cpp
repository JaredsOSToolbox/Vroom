#include <iostream>
#include <memory>
#include <vector>
#include <stdio.h>
#include <list>
#include <chrono>
#include<unistd.h>

#define MAX_REF_COUNT 100
#define FREE_LIST_SIZE 128
#define PAGE_TABLE_SIZE FREE_LIST_SIZE

#define _T template<typename T>

struct time_f {
  long timestamp;

  time_f() {
    set_time();
  }

  void set_time() {
    /*
     * SOURCE
     *
     * https://stackoverflow.com/questions/31255486/c-how-do-i-convert-a-stdchronotime-point-to-long-and-back
     *
     * Get time in milliseconds from Jan 1, 1970
     * NOT COMPLIANT WITH WINDOWS, NOT SORRY
    */

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    auto value = now_ms.time_since_epoch();
    timestamp = value.count();
  }
  long get_time() {
    set_time(); // update the time whenever we access it
    return timestamp;
  }
};

typedef struct time_f time_f;

_T

struct entry_t {
  T data; // data used (in our case this will be addresses)
  uint8_t bit; // dirty bit
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
    last_accessed = 0;
  }
  bool operator==(entry_t<T>* object) {
    return (object == &this);
  }
};

//typedef struct entry_t<T> entry_t<T>;

_T

class page_table_t {
  public:
    page_table_t() {
      this->entries = std::vector<struct entry_t<T>*>();
      this->in_use = std::list<struct entry_t<T>*>();
      this->timer = time_f();
    }

    struct entry_t<T>* operator[](size_t index) {
      struct entry_t<T>* obj = this->entries[index];


      obj->reference_count++;
      obj->last_accessed = this->timer.get_time();
      std::cout << "[INFO] Element accessed: " << obj->last_accessed << std::endl;

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

    struct entry_t<T>* get_entry(size_t index) {
      /*
       * Use this method for checking the timestamp
       * Think of this as a weak pointer
      */

      return this->entries[index];
    }

    void insert(struct entry_t<T>* entry, int position) {
      this->entries.insert(this->entries.begin() + position, entry);
      this->in_use.insert(this->in_use.begin(), entry);
    }
    void check_for_stale_entry() {
      /*
       * We need to prune entries in the use_list if they have
       * not been accessed in the last 200 milliseconds
      */

      long current_time = this->timer.get_time();
      std::cout << "current_time: " << current_time << std::endl;

      std::vector<typename std::list<entry_t<T>*>::iterator> _remove_me;

      for(auto it = this->in_use.begin(); it != this->in_use.end(); ++it) {
        long duration_ = (current_time - (*it)->last_accessed);
        if(duration_ >= 200) {
          std::cout << "we need to remove this object!" << std::endl;
          _remove_me.push_back(
              it);  // we need to put them in a buffer to remove them after
                    // iterating over the list. There might be multiple matches
        }
      }

      for(auto garbage : _remove_me) { this->in_use.erase(garbage); }
    }

  private:
    std::vector<struct entry_t<T>*> entries;
    std::list<struct entry_t<T>*> in_use;
    struct time_f timer; // get current time stamp
}; 

int main(void) {
  unsigned int microsecond = 1000000;

  page_table_t<uint32_t> t = page_table_t<uint32_t>();

  struct entry_t<uint32_t>* entity = new entry_t((uint32_t)100);

  t.insert(entity, 0);
  //auto element = t[0];
  usleep(1 * microsecond);
  t.check_for_stale_entry();
  //delete entity;

  //for(int i = 0; i < 107; ++i) {
    //entry_t<uint32_t>* element = t[0];
    //if(element != nullptr){
        //std::cout << element->reference_count  << std::endl;
        //std::cout  << "max reference count is " << element->maximum_referernces << std::endl;
    //} else {
      //std::cout << "freeing from the free list" << std::endl;
      //break;
    //}
  //}

  return 0;
}

