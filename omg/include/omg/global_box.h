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
  GlobalBox() : m_initialized(false) {}
#endif
  template <typename... Args> void init(Args... args) {
#if ASSERTIONS
    assert(!m_initialized);
#endif
    new (m_buffer) T(args...);
#if ASSERTIONS
    m_initialized = true;
#endif
  }
  void shutdown() {
#if ASSERTIONS
    assert(m_initialized);
#endif
    // TODO: Placement delete
#if ASSERTIONS
    m_initialized = false;
#endif
  }

  T * get() {
#if ASSERTIONS
    assert(m_initialized);
#endif
    return reinterpret_cast< T*>(m_buffer);
  }

  T * operator->() {
    return get();
  }

  operator T*() {
    return get();
  }

private:
  uint8_t m_buffer[sizeof(T)];
#if ASSERTIONS
  bool m_initialized;
#endif
};

}

#endif
