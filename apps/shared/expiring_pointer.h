#ifndef SHARED_EXPIRING_POINTER_H
#define SHARED_EXPIRING_POINTER_H

#include <escher.h>

namespace Shared {

template <class T>
class ExpiringPointer {
  template<typename U>
  friend class ExpiringPointer;
public:
  ExpiringPointer(T * rawPointer, bool reinitGlobal = true) : m_rawPointer(rawPointer) {
#if DEBUG
    s_global = rawPointer;
#endif
  }
  template<class U>
  ExpiringPointer(ExpiringPointer<U> p) : m_rawPointer(static_cast<T *>(p.m_rawPointer)) {
#if DEBUG
    s_global = m_rawPointer;
#endif
  }
  bool isNull() { return m_rawPointer == nullptr; }
  T *operator->() {
#if DEBUG
    assert(m_rawPointer != nullptr && m_rawPointer == s_global);
#endif
    return m_rawPointer;
  }
  T &operator*() {
#if DEBUG
    assert(m_rawPointer != nullptr && m_rawPointer == s_global);
#endif
    return *m_rawPointer;
  }
private:
#if DEBUG
  static T * s_global;
#endif
  T * m_rawPointer;
};

#if DEBUG
template<class T>
T * ExpiringPointer<T>::s_global = nullptr;
#endif

}

#endif
