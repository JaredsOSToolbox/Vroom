#pragma once

#include <stdint.h>
#include <vector>

#define FRAME_SIZE  256

class address_t {
  public:
    address_t(uint32_t, int);
    address_t();

    uint32_t get_logical_address();
    unsigned get_offset();
    unsigned get_page_number();
    unsigned get_physical_address();

    // FIXME
    unsigned calculate_physical_address();
  private:
    uint32_t logical_address;
    unsigned offset, page_number, physical_address, frame;
};
