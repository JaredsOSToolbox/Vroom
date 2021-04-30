#pragma once
#include <queue>

namespace circular {

  template <typename T>
  class queue {
    public:
      queue();
      queue(const std::initializer_list<T>&);

      void push(T);
      T pop();
      size_t size();
      bool empty();

    private:
      std::queue<T> container;
  };
}

#include "queue.tcc"
