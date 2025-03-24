#ifndef ION_DEVICE_SHARED_USB_STREAMABLE_H
#define ION_DEVICE_SHARED_USB_STREAMABLE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

class Streamable {
 public:
  uint16_t copy(void* target, size_t maxSize) const;

 protected:
  class Channel {
   public:
    Channel(void* pointer, size_t maxSize)
        : m_pointer(static_cast<uint8_t*>(pointer)), m_sizeLeft(maxSize) {}
    template <typename T>
    void push(T data) {
      if (m_sizeLeft >= sizeof(T)) {
        memcpy(m_pointer, &data, sizeof(T));
        m_pointer += sizeof(T);
        m_sizeLeft -= sizeof(T);
      }
    }

    template <typename T>
      requires(sizeof(T) == 1)
    void push(T data) {
      if (m_sizeLeft >= 1) {
        *m_pointer++ = *reinterpret_cast<const uint8_t*>(&data);
        m_sizeLeft -= 1;
      }
    }

    size_t sizeLeft() { return m_sizeLeft; }

   private:
    uint8_t* m_pointer;
    size_t m_sizeLeft;
  };
  virtual void push(Channel* c) const = 0;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif
