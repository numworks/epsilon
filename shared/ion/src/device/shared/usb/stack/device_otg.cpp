#include <config/internal_flash.h>
#include <shared/regs/otg.h>

#include "device.h"

namespace Ion {
namespace Device {
namespace USB {

using namespace Regs;

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

    if (pktsts == OTG::GRXSTSP::PKTSTS::OutTransferCompleted ||
        pktsts == OTG::GRXSTSP::PKTSTS::SetupTransactionCompleted) {
      // There is no data associated with this interrupt.
      return;
    }

    assert(pktsts != OTG::GRXSTSP::PKTSTS::GlobalOutNAK);
    /* We did not enable the GONAKEFFM (Global OUT NAK effective mask) bit in
     * GINTSTS, so we should never get this interrupt. */

    assert(pktsts == OTG::GRXSTSP::PKTSTS::OutReceived ||
           pktsts == OTG::GRXSTSP::PKTSTS::SetupReceived);

    TransactionType type = (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived)
                               ? TransactionType::Out
                               : TransactionType::Setup;

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
  if (OTG.DIEPINT(0)->getXFRC()) {  // We only check endpoint 0.
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
    flushFIFOs();
    m_ep0.setup();
    /* In setup(), we should set the MPSIZ field in OTG_DIEPCTL0 to the maximum
     * packet size depending on the enumeration speed (found in OTG_DSTS). We
     * should always get FullSpeed, so we set the packet size accordingly. */
  }
}
bool Device::isSoftDisconnected() const { return OTG.DCTL()->getSDIS(); }

void Device::detach() {
  // Get in soft-disconnected state
  OTG.DCTL()->setSDIS(true);
}

void Device::setAddress(uint8_t address) { OTG.DCFG()->setDAD(address); }

}  // namespace USB
}  // namespace Device
}  // namespace Ion
