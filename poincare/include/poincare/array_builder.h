#ifndef POINCARE_ARRAY_BUILDER_H
#define POINCARE_ARRAY_BUILDER_H


namespace Poincare {

template <class T>
class ArrayBuilder1 {
public:
  ArrayBuilder1(T e1) :
    m_data{e1}
  {}
  T * array() { return const_cast<T *>(m_data); }
private:
  T m_data[1];
};

template <class T>
class ArrayBuilder2 {
public:
  ArrayBuilder2(T e1, T e2) :
    m_data{e1, e2}
  {}
  T * array() { return const_cast<T *>(m_data); }
private:
  T m_data[2];
};

template <class T>
class ArrayBuilder3 {
public:
  ArrayBuilder3(T e1, T e2, T e3) :
    m_data{e1, e2, e3}
  {}
  T * array() { return const_cast<T *>(m_data); }
private:
  T m_data[3];
};

template <class T>
class ArrayBuilder4 {
public:
  ArrayBuilder4(T e1, T e2, T e3, T e4) :
    m_data{e1, e2, e3, e4}
  {}
  T * array() { return const_cast<T *>(m_data); }
private:
  T m_data[4];
};

}

#endif
