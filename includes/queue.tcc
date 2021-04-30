namespace circular {
  template <typename T>
  queue<T>::queue(){}

  template <typename T>
  queue<T>::queue(const std::initializer_list<T>& _list) {
    for(const T& element : _list) {
      this->container.push(element);
    }
  }

  template <typename T>
  void queue<T>::push(T value){
    this->container.push(value);
  }

  template <typename T>
  T queue<T>::pop(){
    T value = this->container.front();
    this->container.pop();
    this->container.push(value);
    return value;
  }

  template <typename T>
  size_t queue<T>::size() { return this->container.size(); }

  template <typename T>
  bool queue<T>::empty(){ return this->container.empty(); }
}

