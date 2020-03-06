#ifndef POINCARE_ARRAY_BUILDER_H
#define POINCARE_ARRAY_BUILDER_H


namespace Poincare {

template <class T>
class ArrayBuilder {
public:
  ArrayBuilder(T e1 = T(), T e2 = T(), T e3 = T(), T e4 = T()) :
    m_data{e1, e2, e3, e4}
  {}
  T * array() { return const_cast<T *>(m_data); }
private:
  T m_data[4];
};

}

#endif
