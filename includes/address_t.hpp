#pragma once

#include <stdint.h>
#include <vector>

class address_t {
  public:
    address_t(uint32_t);
    address_t();

    uint32_t get_value();
    unsigned get_offset();
    unsigned get_page_number();

  private:
    uint32_t value;
    unsigned offset, page_number;
};
