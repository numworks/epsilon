#ifndef ION_DEVICE_SHARED_USB_STREAMABLE_H
#define ION_DEVICE_SHARED_USB_STREAMABLE_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace USB {

class Streamable {
 public:
  uint16_t copy(void *target, size_t maxSize) const;

 protected:
  class Channel {
   public:
    Channel(void *pointer, size_t maxSize)
        : m_pointer(pointer), m_sizeLeft(maxSize) {}
    template <typename T>
    void push(T data) {
      if (m_sizeLeft >= sizeof(T)) {
        T *typedPointer = static_cast<T *>(m_pointer);
        *typedPointer++ = data;  // Actually push the data
        m_pointer = static_cast<void *>(typedPointer);
        m_sizeLeft -= sizeof(T);
      }
    }
    size_t sizeLeft() { return m_sizeLeft; }

   private:
    void *m_pointer;
    size_t m_sizeLeft;
  };
  virtual void push(Channel *c) const = 0;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif
