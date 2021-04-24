#pragma once

#include <string>
#include <vector>
#include <regex>
#include <stdint.h>

#include "address_t.hpp"
#include "validate_t.hpp"

#define VROOM_PAGE_SIZ 256
#define ENTRY_COUNT 256

/*
 * BASE CLASS
*/

class reader_t {
  public:
    explicit reader_t(std::string);
    explicit reader_t();

    std::vector<std::string> read_content();
    std::vector<std::string> content_() const;
    std::string path_() const;
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

/*
 * Read in the binary data file
*/

class backing_store_reader_t : public reader_t {
  // FIXME
  public:
    backing_store_reader_t(std::string);
    ~backing_store_reader_t(); // free the file pointer

    signed char operator[](size_t) const;
    signed char& operator[](size_t);

    void seek_buffer(size_t); // go to certain portion of the backing store file
    signed char get_buffer() const;

  private:
    signed char buffer[FRAME_SIZE];
    FILE* _file_pointer;
};
