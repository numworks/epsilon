#ifndef ION_DEVICE_N0110_KERNEL_DRIVERS_CONFIG_PERSISTING_BYTES_H
#define ION_DEVICE_N0110_KERNEL_DRIVERS_CONFIG_PERSISTING_BYTES_H

namespace Ion {
namespace Device {
namespace PersistingBytes {
namespace Config {

// TODO: factorize the macro with equivalent macro on N100

#define byte4 0xFF, 0xFF, 0xFF, 0xFF
#define byte8 byte4, byte4
#define byte16 byte8, byte8
#define byte32 byte16, byte16
#define byte64 byte32, byte32
#define byte128 byte64, byte64
#define byte256 byte128, byte128
#define byte512 byte256, byte256
#define byte1K byte512, byte512
#define byte2K byte1K, byte1K
#define byte4K byte2K, byte2K

#define PERSISTING_BYTES_BUFFER_CONTENT byte4K

constexpr static int BufferSize = 4*1024;

}
}
}
}

#endif

