#pragma once

#include <array>
#include <stdbool.h>

class tlb_t {
  public:
    int query_page_number(unsigned);
    int query_frame_number(unsigned);

    std::array<int> get_page_number_buffer();
    std::array<int> get_frame_number_buffer();

    bool is_full();

  private:
    std::array<int> page_number_buffer, frame_number_buffer; // we are using an array to just save space
};
