#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 128

class address_t {
  public:
    address_t(uint32_t, int);
    address_t();

    uint32_t get_logical_address() const;
    unsigned get_offset() const;
    unsigned get_page_number() const;
    unsigned get_physical_address() const;
    unsigned get_frame() const;
    int get_value() const;

    // FIXME
    unsigned calculate_physical_address();
    friend std::ostream& operator<<(std::ostream&, const address_t&);
    bool operator==(const address_t&) const;

  private:
    uint32_t logical_address;
    unsigned offset, page_number, physical_address, frame;
    int value;
};
