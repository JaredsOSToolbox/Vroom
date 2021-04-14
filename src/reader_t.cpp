#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <inttypes.h>

#include "../includes/reader_t.hpp"
#include "../includes/address_t.hpp"

reader_t::reader_t(std::string path) {
  this->path = path;
  this->content = this->read_contents();
}

reader_t::reader_t() {
  this->path = "";
  this->content = std::vector<std::string>();
  this->parsed_contents = std::vector<address_t>();
}

std::string reader_t::get_path() { return this->path; }

std::vector<std::string> reader_t::get_content() { return this->content; }
std::vector<address_t> reader_t::get_parsed_contents(){ return this->parsed_contents; }

size_t reader_t::get_length() { return this->content.size(); }

address_t reader_t::process_line(int frame, std::string line) {
  /*
   * FIXME
  */

  return address_t(
    (uint32_t)std::strtoul(line.c_str(), NULL, 10),
    frame
  );

  //std::cout << "[INFO] line: " << line << std::endl;
  //std::vector<std::string> contents {
    //std::sregex_token_iterator(line.begin(), line.end(), re, 1),
    //std::sregex_token_iterator()
  //};

  //std::cout << "[INFO] Amount of matches " << contents.size() << std::endl;

  //std::vector<uint32_t> container;
  //uint32_t a;

  //for(std::string str : contents) {

    //std::cout << "[INFO] Processing " << str << std::endl;
    //a = std::strtoul(str.c_str(), NULL, 10);
    //container.push_back(
      //(uint32_t)std::strtoul(str.c_str(), NULL, 10)
    //);
  //}
  //return container;
}

std::vector<std::string> reader_t::read_contents() {
  std::ifstream file;
  file.open(this->path);
  std::vector<std::string> contents;

  if (!file.is_open()) {
    throw std::invalid_argument("cannot open file!");
  }

  std::vector<address_t> _processed; 
  int i = 0;

  for (std::string line; std::getline(file, line);) {
    if(line.size() == 0){
      // empty line
      continue;
    }

    _processed.push_back(process_line(i++, line));
    //switch(flag) {
      //case ADDRESSES:
       //_processed = this->process_line(line, std::regex("[0-9]+"));
       //break;
      //case COMPLETED:
        //_processed = this->process_line(
            //line,
            //std::
                //regex("Virtual address: [0-9]+ Physical address: [0-9]+ Value: [0-9]+"));
        //break;
    //}
    //this->parsed_contents.insert(this->parsed_contents.end(), _processed.begin(), _processed.end());
  }
  this->parsed_contents = _processed;
  file.close();
  return contents;
}

std::ostream& operator<<(std::ostream& os, const reader_t& reader){
  //for(std::string line : reader.get_content()){
    //os << line << std::endl;
  //}
  return os;
}
