#include <assert.h>
#include <stm32u083/usb.h>
#include <stm32u083/usb_sram.h>

#include "device.h"
#include "helpers_usbfs.h"

namespace Ion {
namespace Device {
namespace USB {

using STM32U083::USB_SRAM;
using USBFS = STM32U083::USB;

void Device::poll() {
  // SETUP or OUT transaction
  if (USBFS::CHEP0R::Read().getVTRX()) {
    CHEP0R::ClearVTRX();
    // We only use endpoint 0
    assert(USBFS::ISTR::Read().getIDN() == 0);

    TransactionType type = (!USBFS::CHEP0R::Read().getSETUP())
                               ? TransactionType::Out
                               : TransactionType::Setup;

    if (type == TransactionType::Setup &&
        CHEP0R::GetSTATTX() == Status::Valid) {
      // SETUP received but there is a packet in the Tx FIFO. Flush it.
      m_ep0.flushTxFifo();
    }

    // Save the received packet byte count
    m_ep0.setReceivedPacketSize(USB_SRAM::CHEP_RXTXBD_0::Read().getCOUNT_RX());

    if (type == TransactionType::Setup) {
      m_ep0.readAndDispatchSetupPacket();
    } else {
      assert(type == TransactionType::Out);
      m_ep0.processOUTpacket();
    }

    m_ep0.discardUnreadData();
    assert(USBFS::CHEP0R::Read().getVTRX() == false);
    assert(CHEP0R::GetSTATRX() == Status::Valid ||
           CHEP0R::GetSTATTX() == Status::Valid);
  }

  // IN transaction
  if (USBFS::CHEP0R::Read().getVTTX()) {
    CHEP0R::ClearVTTX();
    m_ep0.processINpacket();

    if (m_addressToSet) {
      STM32U083::USB::DADDR::Read().setADD(m_addressToSet).write();
      m_addressToSet = 0;
    }

    assert(CHEP0R::GetSTATRX() == Status::Valid ||
           CHEP0R::GetSTATTX() == Status::Valid);
  }

  // TODO: Handle USB RESET
}

bool Device::isSoftDisconnected() const {
  // SUSB::BCDR::Read().getDPPU_DPD();
  return m_softDisconnect;
}

void Device::detach() {
  // Get in soft-disconnected state
  // SUSB::BCDR::Read().setDPPU_DPD(false).write();
  m_softDisconnect = true;
}

void Device::setAddress(uint8_t address) { m_addressToSet = address; }

}  // namespace USB
}  // namespace Device
}  // namespace Ion
