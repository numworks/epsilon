#ifndef STORE_H
#define STORE_H

class Store {
public:
  Store();
  // Setters
  void push(int number);
  int add(int number);
  int subtract(int number);
  // Getters
  const int value(int index) const;
  constexpr static int k_maxNumberOfStoredValues = 3;
};

#endif
