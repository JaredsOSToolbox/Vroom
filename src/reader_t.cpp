#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <inttypes.h>
#include <stdio.h>

#include <limits>

#include "../includes/reader_t.hpp"
#include "../includes/address_t.hpp"
#include "../includes/validate_t.hpp"

/*
 * BEGIN READER_T BASE
*/

reader_t::reader_t(std::string path) {
  this->path = path;
  this->content = this->read_content();
}

reader_t::reader_t() {
  this->path = "";
  this->content = std::vector<std::string>();
}

std::vector<std::string> reader_t::read_content() {
  std::ifstream file;
  file.open(this->path);
  std::vector<std::string> contents;

  if (!file.is_open()) {
    throw std::invalid_argument("cannot open file!");
  }

  for (std::string line; std::getline(file, line);) {
    if(line.size() == 0){ continue; }
    contents.push_back(line);
  }
  file.close();
  return contents;
}

std::vector<std::string> reader_t::content_() const { return this->content; }
std::string reader_t::path_() const { return this->path; }

std::ostream& operator<<(std::ostream& os, const reader_t& reader){
  for(std::string line : reader.content_()){
    os << line << std::endl;
  }
  return os;
}

/*
 * END READER_T BASE
*/

/*
 * BEGIN ADDRESS_READER_T BASE
*/

uint32_t convert(std::string str) {
  return (uint32_t)std::strtol(str.c_str(), NULL, 10);
}

std::vector<address_t> address_reader_t::get_content() const {
  return this->parsed_contents;
}

size_t address_reader_t::size() const { return this->get_content().size(); }

void address_reader_t::process_line(std::string line) {
  this->parsed_contents.emplace_back(
      address_t(convert(line)));
}

void address_reader_t::produce_parsed_contents() {
  for(std::string line : this->content_()) {
    this->process_line(line);
  }
}

address_t& address_reader_t::operator[](size_t index) {
  return this->parsed_contents[index];
}

address_t address_reader_t::operator[](size_t index) const {
  return this->parsed_contents[index];
}

/*
 * END ADDRESS_READER_T BASE
*/

/*
 * BEGIN VALIDATE_READER_T BASE
*/

void validate_reader_t::process_line(std::string line) {
  std::regex _re = std::regex(
      "Virtual address: ([0-9]+) Physical address: ([0-9]+) Value: (\\-?[0-9]+)");

  std::vector<address_t> container;

  for (auto it = std::sregex_iterator(line.begin(), line.end(), _re);
       it != std::sregex_iterator(); it++) {

    std::smatch sm = *it;
    this->parsed_contents.emplace_back(
      validate_t(convert(sm.str(1)), convert(sm.str(2)), convert(sm.str(3)))
    );

  }
}

void validate_reader_t::process_content() {
  for(std::string line : this->content_()){
    this->process_line(line);
  }
}

bool validate_reader_t::validate_lookup(int index, address_t address) {
  // FIXME | write overloaded operator for address_t and validate_t
  return true;
}

std::vector<validate_t> validate_reader_t::get_content() const {
  return this->parsed_contents;
}


validate_t& validate_reader_t::operator[](size_t index) {
  return this->parsed_contents[index];
}

validate_t validate_reader_t::operator[](size_t index) const {
  return this->parsed_contents[index];
}

bool validate_reader_t::operator==(const address_reader_t& other) {
  if(other.size() != this->parsed_contents.size()) { return false; } 

  for(size_t i = 0; i < other.size(); i++) {
    if(this->parsed_contents[i] != other[i]){
      return false;
    } else {
      std::cout << other[i] << " passed" << std::endl;
    }
  }
  return true;
}

/*
 * END VALIDATE_READER_T
*/

/*
 * BEGIN BACKING_STORE_READER_T
*/

backing_store_reader_t::backing_store_reader_t(std::string path){
  this->_file_pointer = fopen(path.c_str(), "rb");
}

backing_store_reader_t::~backing_store_reader_t(){
  fclose(this->_file_pointer);
}

signed char& backing_store_reader_t::operator[](size_t offset) {
  return this->buffer[offset];
}

signed char backing_store_reader_t::operator[](size_t offset) const {
  return this->buffer[offset];
}

void backing_store_reader_t::seek_buffer(size_t position) {
  if(std::fseek(this->_file_pointer, position * PAGE_SIZE, SEEK_SET) != 0) {
    throw std::logic_error("[FATAL] Error while seeking backing store");
  }
  if(std::fread(this->buffer, sizeof(signed char), FRAME_SIZE, this->_file_pointer) == 0) {
    throw std::logic_error("[FATAL] Error while reading backing store");
  } 
}

signed char* backing_store_reader_t::get_buffer() { return this->buffer; }


/*
 * END BACKING_STORE_READER_T
*/

