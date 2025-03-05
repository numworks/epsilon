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
private:
  // TODO: use lambda function for a cleaner code
  int operate(int number, bool add);
  // Model
  int m_storedValues[k_maxNumberOfStoredValues];
  int m_currentIndex;
};

#endif
