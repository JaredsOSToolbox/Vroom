#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 256

class address_t {
  public:
    address_t(uint32_t);
    address_t();

    uint32_t get_logical_address() const;
    unsigned get_offset() const;
    unsigned get_page_number() const;
    unsigned get_physical_address() const;
    unsigned get_frame() const;
    int get_value() const;
    void assign_frame(int);
    void assign_value(int);

    // FIXME
    unsigned calculate_physical_address() const;
    friend std::ostream& operator<<(std::ostream&, const address_t&);
    bool operator==(const address_t&) const;

  private:
    uint32_t logical_address;
    unsigned offset, page_number, physical_address;
    int value, frame;
};
