#pragma once

#include <string>
#include <vector>
#include <regex>
#include <stdint.h>

#include "address_t.hpp"

#define ADDRESSES 0
#define COMPLETED 1

class reader_t {
  public:
    reader_t(std::string);
    reader_t();

    std::string get_path();
    size_t get_length();

    std::vector<std::string> get_content();
    std::vector<address_t> get_parsed_contents();

    std::vector<std::string> read_contents();

    address_t process_line(int, std::string);

    friend std::ostream& operator<<(std::ostream&, const reader_t&);

  private:
    std::string path;
    std::vector<std::string> content;
    std::vector<address_t> parsed_contents;

};

