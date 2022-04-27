#include <bootloader/usb_data.h>
#include <bootloader/interface/static/messages.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
}

static char data[255];

const char * Bootloader::USBData::buildStringDescriptor(StringHeader header, uint32_t startAddress, uint32_t size) {
    strlcpy(data, header.getString(), sizeof(data));
    itoa((int32_t)startAddress, &data[strlen(header.getString())], 16);
    data[strlen(header.getString()) + 8] = '/';
    data[strlen(header.getString()) + 8 + 1] = '0';
    data[strlen(header.getString()) + 8 + 2] = '1';
    data[strlen(header.getString()) + 8 + 3] = '*';
    data[strlen(header.getString()) + 8 + 4] = '0';
    itoa((int32_t)size/1024, &data[strlen(header.getString()) + 8 + 5], 10);
    data[strlen(header.getString()) + 8 + 5 + 2] = 'K';
    data[strlen(header.getString()) + 8 + 5 + 2 + 1] = 'g';
    data[strlen(header.getString()) + 8 + 5 + 2 + 2] = '\0';
    return &data[0];
}

const Bootloader::USBData Bootloader::USBData::DEFAULT() {
    return USBData("@Flash/0x90000000/08*004Kg,01*032Kg,63*064Kg,64*064Kg", Messages::usbUpsilonBootloader, ProtectionState(false, true));
}

const Bootloader::USBData Bootloader::USBData::BOOTLOADER_UPDATE() {
    return USBData("@Flash/0x08000000/04*016Kg", Messages::usbBootloaderUpdate, ProtectionState(true, false));
}

Bootloader::USBData Bootloader::USBData::Recovery(uint32_t startAddress, uint32_t size) {
    return USBData(buildStringDescriptor(StringHeader::SRAM(), startAddress, size), Messages::usbUpsilonRecovery, ProtectionState(false, false));
}
