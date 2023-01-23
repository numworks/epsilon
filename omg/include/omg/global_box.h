#ifndef OMG_GLOBAL_BOX_H
#define OMG_GLOBAL_BOX_H

#include <stdint.h>
#include <new>

/* This template wraps another type and manually control when its constructor
 * is called. It is useful to declare global variables and yet avoid the static
 * initialization order fiasco (by manually controlling when the variable is
 * actually constructed). It also allows a global variable to be reset. */

namespace OMG {

template <typename T> class GlobalBox {
public:
#if ASSERTIONS
  GlobalBox() : m_isInitialized(false) {}
#endif
  template <typename... Args> void init(Args... args) {
#if ASSERTIONS
    assert(!m_isInitialized);
#endif
    new (m_buffer) T(args...);
#if ASSERTIONS
    m_isInitialized = true;
#endif
  }
  void deinit() {
#if ASSERTIONS
    assert(m_isInitialized);
#endif
    get()->~T();
#if ASSERTIONS
    m_isInitialized = false;
#endif
  }

  T * get() {
#if ASSERTIONS
    assert(m_isInitialized);
#endif
    return reinterpret_cast< T*>(m_buffer);
  }
  T * operator->() { return get(); }
  operator T*() { return get(); }

private:
  uint8_t m_buffer[sizeof(T)];
#if ASSERTIONS
  bool m_isInitialized;
#endif
};

template <typename T> class TrackedGlobalBox : public GlobalBox<T> {
public:
  TrackedGlobalBox() : GlobalBox<T>(), m_isInitialized(false) {}
  template <typename... Args> void init(Args... args) {
    if (!m_isInitialized) {
      GlobalBox<T>::init(args...);
      m_isInitialized = true;
    }
  }
  void deinit() {
    if (m_isInitialized) {
      GlobalBox<T>::deinit();
      m_isInitialized = false;
    }
  }

  bool isInitialized() const { return m_isInitialized; }

private:
  bool m_isInitialized;
};

}

#endif
