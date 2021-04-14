#pragma once

#include <string>
#include <vector>
#include <regex>
#include <stdint.h>

#include "address_t.hpp"
#include "validate_t.hpp"

/*
 * BASE CLASS
*/

class reader_t {
  public:
    explicit reader_t(std::string);
    explicit reader_t();

    std::vector<std::string> read_content();
    std::vector<std::string> content_() const;
    friend std::ostream& operator<<(std::ostream&, const reader_t&);

  private:
    std::string path;
    std::vector<std::string> content;
};

/*
 * PARSE ADDRESSES.TXT
*/

class address_reader_t: public reader_t {
  public:
    using reader_t::reader_t;
    void process_line(int, std::string);
    void produce_parsed_contents();

    std::vector<address_t> get_content() const;
    size_t size() const;

    address_t operator[](size_t) const;
    address_t& operator[](size_t);

  private:
    std::vector<address_t> parsed_contents;
};

/*
 * PARSE CORRECT.TXT
*/

class validate_reader_t : public reader_t {
  public:
    using reader_t::reader_t;
    void process_line(std::string);
    void process_content();
    bool validate_lookup(int, address_t);

    std::vector<validate_t> get_content() const;

    validate_t operator[](size_t) const;
    validate_t& operator[](size_t);

    bool operator==(const address_reader_t&);

  private:
    std::vector<validate_t> parsed_contents;
};
