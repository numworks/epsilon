#include "device.h"
#include <config/internal_flash.h>
#include <shared/regs/otg.h>

namespace Ion {
namespace Device {
namespace USB {

using namespace Regs;

static inline uint16_t minUint16T(uint16_t x, uint16_t y) { return x < y ? x : y; }

void Device::poll() {
  // Read the interrupts
  class OTG::GINTSTS intsts(OTG.GINTSTS()->get());

  /* SETUP or OUT transaction
   * If the Rx FIFO is not empty, there is a SETUP or OUT transaction.
   * The interrupt is done AFTER THE HANSDHAKE of the transaction. */
  if (intsts.getRXFLVL()) {
    class OTG::GRXSTSP grxstsp(OTG.GRXSTSP()->get());

    // Store the packet status
    OTG::GRXSTSP::PKTSTS pktsts = grxstsp.getPKTSTS();

    // We only use endpoint 0
    assert(grxstsp.getEPNUM() == 0);

    if (pktsts == OTG::GRXSTSP::PKTSTS::OutTransferCompleted || pktsts == OTG::GRXSTSP::PKTSTS::SetupTransactionCompleted) {
      // There is no data associated with this interrupt.
      return;
    }

    assert(pktsts != OTG::GRXSTSP::PKTSTS::GlobalOutNAK);
    /* We did not enable the GONAKEFFM (Global OUT NAK effective mask) bit in
     * GINTSTS, so we should never get this interrupt. */

    assert(pktsts == OTG::GRXSTSP::PKTSTS::OutReceived || pktsts == OTG::GRXSTSP::PKTSTS::SetupReceived);

    TransactionType type = (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived) ? TransactionType::Out : TransactionType::Setup;

    if (type == TransactionType::Setup && OTG.DIEPTSIZ0()->getPKTCNT()) {
      // SETUP received but there is a packet in the Tx FIFO. Flush it.
      m_ep0.flushTxFifo();
    }

    // Save the received packet byte count
    m_ep0.setReceivedPacketSize(grxstsp.getBCNT());

    if (type == TransactionType::Setup) {
      m_ep0.readAndDispatchSetupPacket();
    } else {
      assert(type == TransactionType::Out);
      m_ep0.processOUTpacket();
    }

    m_ep0.discardUnreadData();
  }

  /* IN transactions.
   * The interrupt is done AFTER THE HANSDHAKE of the transaction. */
  if (OTG.DIEPINT(0)->getXFRC()) { // We only check endpoint 0.
    m_ep0.processINpacket();
    // Clear the Transfer Completed Interrupt
    OTG.DIEPINT(0)->setXFRC(true);
  }

  // Handle USB RESET. ENUMDNE = **SPEED** Enumeration Done
  if (intsts.getENUMDNE()) {
    // Clear the ENUMDNE bit
    OTG.GINTSTS()->setENUMDNE(true);
    /* After a USB reset, the host talks to the device by sending messages to
     * address 0; */
    setAddress(0);
    // Flush the FIFOs
    m_ep0.reset();
    m_ep0.setup();
    /* In setup(), we should set the MPSIZ field in OTG_DIEPCTL0 to the maximum
     * packet size depending on the enumeration speed (found in OTG_DSTS). We
     * should always get FullSpeed, so we set the packet size accordingly. */
  }
}

bool Device::isSoftDisconnected() const {
  return OTG.DCTL()->getSDIS();
}

void Device::detach() {
  // Get in soft-disconnected state
  OTG.DCTL()->setSDIS(true);
}

bool Device::processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  // Device only handles standard requests.
  if (request->requestType() != SetupPacket::RequestType::Standard) {
    return false;
  }
  switch (request->bRequest()) {
    case (int) Request::GetStatus:
      return getStatus(transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (int) Request::SetAddress:
      // Make sure the request is adress is valid.
      assert(request->wValue() < 128);
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      setAddress(request->wValue());
      *transferBufferLength = 0;
      return true;
    case (int) Request::GetDescriptor:
      return getDescriptor(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (int) Request::SetConfiguration:
      *transferBufferLength = 0;
      return setConfiguration(request);
    case (int) Request::GetConfiguration:
      return getConfiguration(transferBuffer, transferBufferLength);
  }
  return false;
}

bool Device::getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  *transferBufferLength = minUint16T(2, transferBufferMaxLength);
  for (int i = 0; i<*transferBufferLength; i++) {
    transferBuffer[i] = 0; // No remote wakeup, not self-powered.
  }
  return true;
}

void Device::setAddress(uint8_t address) {
  OTG.DCFG()->setDAD(address);
}

bool Device::getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  Descriptor * wantedDescriptor = descriptor(request->descriptorType(), request->descriptorIndex());
  if (wantedDescriptor == nullptr) {
    return false;
  }
  *transferBufferLength = wantedDescriptor->copy(transferBuffer, transferBufferMaxLength);
  return true;
}

bool Device::getConfiguration(uint8_t * transferBuffer, uint16_t * transferBufferLength) {
  *transferBufferLength = 1;
  transferBuffer[0] = getActiveConfiguration();
  return true;
}

bool Device::setConfiguration(SetupPacket * request) {
  // We support one configuration only
  setActiveConfiguration(request->wValue());
  /* There is one configuration only, we no need to set it again, just reset the
   * endpoint. */
  m_ep0.reset();
  return true;
}

}
}
}
