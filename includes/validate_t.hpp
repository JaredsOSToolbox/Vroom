#pragma once

#include <stdint.h>
#include <tuple>

#include "address_t.hpp"

class validate_t {
  public:
    validate_t();
    validate_t(uint32_t, uint32_t, uint32_t);

    std::tuple<uint32_t, uint32_t, uint32_t> get_values() const;
    uint32_t get_physical_address() const;
    uint32_t get_value() const;
    bool operator==(const address_t&);
    bool operator==(const int&);
    bool operator!=(const address_t&);

    friend std::ostream& operator<<(std::ostream&, const validate_t&);
  private:
    uint32_t virt, physical, value;
};

