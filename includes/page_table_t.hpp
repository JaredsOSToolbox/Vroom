class page_table_t {
  public:

    int& operator[](size_t);
    int operator[](size_t) const;

  private:
    std::array<int> table;
};
