#pragma once

#include <vector>
#include <tuple>
#include "address_t.hpp"

#define TLB_T_SIZE_LIMIT 15

class tlb_t {
  public:
    tlb_t();
    bool is_full();
    
    void set_dirty_bit(size_t);

    address_t operator[](size_t) const;
    address_t& operator[](size_t);

    void replace_line(size_t, address_t);
    address_t* query_table(address_t);
    std::vector<std::tuple<bool, address_t>> get_cache() const;

    friend std::ostream& operator<<(std::ostream&, const tlb_t&);

  private:
    std::vector<std::tuple<bool, address_t>> cache;
};
