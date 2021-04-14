#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

#define FRAME_SIZE  256

class address_t {
  public:
    address_t(uint32_t, int);
    address_t();

    uint32_t get_logical_address() const;
    unsigned get_offset() const;
    unsigned get_page_number() const;
    unsigned get_physical_address() const;

    // FIXME
    unsigned calculate_physical_address();
    friend std::ostream& operator<<(std::ostream&, const address_t&);

  private:
    uint32_t logical_address;
    unsigned offset, page_number, physical_address, frame;
};
