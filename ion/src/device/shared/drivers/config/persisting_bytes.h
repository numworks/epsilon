#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_PERSISTING_BYTES_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_PERSISTING_BYTES_H

namespace Ion {
namespace Device {
namespace PersistingBytes {
namespace Config {

#define byte4First 0x00, 0xFF, 0xFF, 0xFF
#define byte4 0xFF, 0xFF, 0xFF, 0xFF
#define byte8First byte4First, byte4
#define byte8 byte4, byte4
#define byte16First byte8First, byte8
#define byte16 byte8, byte8
#define byte32First byte16First, byte16
#define byte32 byte16, byte16
#define byte64First byte32First, byte32
#define byte64 byte32, byte32
#define byte128First byte64First, byte64
#define byte128 byte64, byte64
#define byte256First byte128First, byte128
#define byte256 byte128, byte128
#define byte512First byte256First, byte256
#define byte512 byte256, byte256
#define byte1KFirst byte512First, byte512
#define byte1K byte512, byte512
#define byte2KFirst byte1KFirst, byte1K
#define byte2K byte1K, byte1K
#define byte4KFirst byte2K, byte2K
#define byte4K byte2K, byte2K
#define byte8KFirst byte4KFirst, byte4K
#define byte8K byte4K, byte4K
#define byte16KFirst byte8KFirst, byte8K
#define byte16K byte8K, byte8K
#define byte32KFirst byte16KFirst, byte16K
#define byte32K byte16K, byte16K

}
}
}
}

#endif
