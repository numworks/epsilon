#ifndef ION_DEVICE_N0100_REGS_CONFIG_USART_H
#define ION_DEVICE_N0100_REGS_CONFIG_USART_H

namespace Ion {
namespace Device {
namespace USART {
namespace Config {

constexpr static int SROffset = 0x00;
constexpr static int RDROffset = 0x04;
constexpr static int TDROffset = 0x04;
constexpr static int BRROffset = 0x08;
constexpr static int CR1Offset = 0x0C;
constexpr static int UEOffset = 13;

}
}
}
}

#endif
