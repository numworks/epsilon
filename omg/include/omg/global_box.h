#ifndef OMG_GLOBAL_BOX_H
#define OMG_GLOBAL_BOX_H

#include <stdint.h>

/* This template wraps another type and manually control when its constructor
 * is called. It is useful to declare global variables and yet avoid the static
 * initialization order fiasco (by manually controlling when the variable is
 * actually constructed). It also allows a global variable to be reset. */

namespace OMG {

template <typename T> class GlobalBox {
public:
  void init() {
    new (m_buffer) T();
  }
  void shutdown() {
    // TODO: Placement delete
  }
  T* operator->() {
    return reinterpret_cast<T *>(m_buffer);
  }
private:
  uint8_t m_buffer[sizeof(T)];
};

}

#endif
