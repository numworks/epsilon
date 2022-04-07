#ifndef BOOTLOADER_USB_DATA_H_
#define BOOTLOADER_USB_DATA_H_

#include <stdint.h>
#include <stddef.h>

namespace Bootloader {

class DFUData {
  public:
    DFUData(bool unlockInternal = false, bool unlockExternal = true) : m_protectInternal(!unlockInternal), m_protectExternal(!unlockExternal) {};

    bool isProtectedInternal() const { return m_protectInternal; }
    bool isProtectedExternal() const { return m_protectExternal; }

  private:
    bool m_protectInternal;
    bool m_protectExternal;
};

class USBData {
  public:
    class StringHeader{
      public:
        StringHeader(const char * string) : m_string(string) {};

        const char * getString() const { return m_string; }

        static const StringHeader Flash() { return StringHeader("@Flash/0x"); }
        static const StringHeader SRAM() { return StringHeader("@SRAM/0x"); }

      private:
        const char * m_string;
    };

    USBData(const char * desc, const char * name, DFUData data = DFUData()) : m_stringDescriptor(desc), m_name(name), m_data(&data) {};

    const char * stringDescriptor() const { return m_stringDescriptor; }
    const char * getName() const { return m_name; }
    DFUData * getData() const { return m_data; }

    static const char * buildStringDescriptor(StringHeader header, uint32_t startAddress, uint32_t size);

    static const USBData DEFAULT();
    static const USBData BOOTLOADER_UPDATE();
    static USBData Recovery(uint32_t startAddress, uint32_t size);

  private:
    const char * m_stringDescriptor;
    const char * m_name;
    DFUData * m_data;

};
}

#endif //BOOTLOADER_USB_DATA_H_