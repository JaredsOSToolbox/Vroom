#pragma once

#include <stdint.h>
#include <tuple>

#include "address_t.hpp"

class validate_t {
  public:
    validate_t();
    validate_t(uint32_t, uint32_t, uint32_t);

    std::tuple<uint32_t, uint32_t, uint32_t> get_values();
    //bool operator==(const address_t&);
  private:
    uint32_t virt, physical, value;
};

