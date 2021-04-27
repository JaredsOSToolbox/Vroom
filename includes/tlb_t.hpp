#pragma once

#include <vector>
#include <queue>
#include "address_t.hpp"
#include "page_table_t.hpp"

#define TLB_T_SIZE_LIMIT 16

class tlb_t {
  public:
    tlb_t();
    bool is_full();
    
    //void set_dirty_bit(size_t);

    entry::entry_t<address_t, signed char*>* operator[](size_t) const;
    //entry::entry_t<address_t, signed char*>& operator[](size_t); FIXME

    void insert(size_t, entry::entry_t<address_t, signed char*>*);
    entry::entry_t<address_t, signed char*>* query_table(entry::entry_t<address_t, signed char*>*);

    std::vector<entry::entry_t<address_t, signed char*>*> get_cache() const;
    void prune_cache();

    friend std::ostream& operator<<(std::ostream&, const tlb_t&);
    size_t slot_available();
    size_t size();

  private:
    std::vector<entry::entry_t<address_t, signed char*>*> cache;
    std::queue<size_t> available_slots;
};
