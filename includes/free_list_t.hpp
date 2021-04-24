#include <list>
#include <iostream>

class free_list_t {
  public:
    free_list_t();

    friend std::ostream& operator<<(std::ostream&, const free_list_t&);
    int pop();
    void push(int);

  private:
    std::list<int> free_frames;
};
