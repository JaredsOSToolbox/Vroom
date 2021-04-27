#pragma once

#include <vector>
#include <queue>
#include "address_t.hpp"
#include "page_table_t.hpp"

#define TLB_T_SIZE_LIMIT 15
#define STALE_LIMIT 200
// ^ in milliseconds

class tlb_t {
  public:
    tlb_t();
    bool is_full();
    
    //void set_dirty_bit(size_t);

    entry::entry_t<address_t>* operator[](size_t) const;
    //entry::entry_t<address_t>& operator[](size_t); FIXME

    void insert(size_t, entry::entry_t<address_t>*);
    entry::entry_t<address_t>* query_table(entry::entry_t<address_t>*);

    std::vector<entry::entry_t<address_t>*> get_cache() const;
    void prune_cache();

    friend std::ostream& operator<<(std::ostream&, const tlb_t&);
    size_t slot_available();

  private:
    std::vector<entry::entry_t<address_t>*> cache;
    std::queue<size_t> available_slots;
};
