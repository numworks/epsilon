#ifndef SHARED_EXPIRING_POINTER_H
#define SHARED_EXPIRING_POINTER_H

#include <escher.h>

namespace Shared {

template <class T>
class ExpiringPointer {
  template<typename U>
  friend class ExpiringPointer;
public:
  ExpiringPointer(T * rawPointer) : m_rawPointer(rawPointer) {
#ifndef NDEBUG
    s_global = rawPointer;
#endif
  }
  T * pointer() { return m_rawPointer; }
  T *operator->() {
#ifndef NDEBUG
    assert(m_rawPointer != nullptr && m_rawPointer == s_global);
#endif
    return m_rawPointer;
  }
  T &operator*() {
#ifndef NDEBUG
    assert(m_rawPointer != nullptr && m_rawPointer == s_global);
#endif
    return *m_rawPointer;
  }
private:
#ifndef NDEBUG
  static T * s_global;
#endif
  T * m_rawPointer;
};

#ifndef NDEBUG
template<class T>
T * ExpiringPointer<T>::s_global = nullptr;
#endif

}

#endif
