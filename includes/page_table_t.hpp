#include "reader.hpp"

class page_table_t {
  public:

    uint32_t& operator[](size_t);
    uint32_t operator[](size_t) const;

  private:
    backing_store_reader_t RAM;
};
