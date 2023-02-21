#include "endpoint0.h"

#include <shared/regs/otg.h>
#include <string.h>

#include <algorithm>

#include "device.h"
#include "interface.h"
#include "request_recipient.h"

namespace Ion {
namespace Device {
namespace USB {

using namespace Regs;

constexpr int Endpoint0::k_maxPacketSize;
constexpr uint16_t Endpoint0::MaxTransferSize;

void Endpoint0::setup() {
  // Setup the IN direction

  // Reset the device IN endpoint 0 transfer size register
  class OTG::DIEPTSIZ0 dieptsiz0(0);
  /* Transfer size. The core interrupts the application only after it has
   * exhausted the transfer size amount of data. The transfer size is set to the
   * maximum packet size, to be interrupted at the end of each packet. */
  dieptsiz0.setXFRSIZ(k_maxPacketSize);
  OTG.DIEPTSIZ0()->set(dieptsiz0);

  // Reset the device IN endpoint 0 control register
  class OTG::DIEPCTL0 diepctl0(0);  // Reset value
  // Set the maximum packet size
  diepctl0.setMPSIZ(OTG::DIEPCTL0::MPSIZ::Size64);
  // Set the NAK bit: all IN transactions on endpoint 0 receive a NAK answer
  diepctl0.setSNAK(true);
  // Enable the endpoint
  diepctl0.setEPENA(true);
  OTG.DIEPCTL0()->set(diepctl0);

  // Setup the OUT direction

  setupOut();
  // Set the NAK bit
  OTG.DOEPCTL0()->setSNAK(true);
  // Enable the endpoint
  enableOut();

  // Setup the Tx FIFO

  /* Tx FIFO depth
   * We process each packet as soon as it arrives, so we only need
   * k_maxPacketSize bytes. TX0FD being in terms of 32-bit words, we divide
   * k_maxPacketSize by 4. */
  OTG.DIEPTXF0()->setTX0FD(k_maxPacketSize / 4);
  /* Tx FIFO RAM start address. It starts just after the Rx FIFOso the value is
   * Rx FIFO start address (0) + Rx FIFO depth. the Rx FIFO depth is set in
   * usb.cpp, but because the code is linked separately, we cannot get it. */
  OTG.DIEPTXF0()->setTX0FSA(128);
}

void Endpoint0::setupOut() {
  class OTG::DOEPTSIZ0 doeptsiz0(0);
  // Number of back-to-back SETUP data packets the endpoint can receive
  doeptsiz0.setSTUPCNT(1);
  // Packet count, false if a packet is written into the Rx FIFO
  doeptsiz0.setPKTCNT(true);
  /* Transfer size. The core interrupts the application only after it has
   * exhausted the transfer size amount of data. The transfer size is set to the
   * maximum packet size, to be interrupted at the end of each packet. */
  doeptsiz0.setXFRSIZ(64);
  OTG.DOEPTSIZ0()->set(doeptsiz0);
}

void Endpoint0::setOutNAK(bool nak) {
  m_forceNAK = nak;
  /* We need to keep track of the NAK state of the endpoint to use the value
   * after a setupOut in poll() of device.cpp. */
  if (nak) {
    OTG.DOEPCTL0()->setSNAK(true);
  } else {
    OTG.DOEPCTL0()->setCNAK(true);
  }
}

void Endpoint0::enableOut() { OTG.DOEPCTL0()->setEPENA(true); }

void Endpoint0::reset() {
  flushTxFifo();
  flushRxFifo();
}

void Endpoint0::readAndDispatchSetupPacket() {
  setOutNAK(true);

  // Read the 8-bytes Setup packet
  if (readPacket(m_largeBuffer, sizeof(SetupPacket)) != sizeof(SetupPacket)) {
    stallTransaction();
    return;
  };

  m_request = SetupPacket(m_largeBuffer);
  uint16_t maxBufferLength = std::min(m_request.wLength(), MaxTransferSize);

  // Forward the request to the request recipient
  uint8_t type = static_cast<uint8_t>(m_request.recipientType());
  if (type == 0) {
    // Device recipient
    m_requestRecipients[0]->processSetupRequest(
        &m_request, m_largeBuffer, &m_transferBufferLength, maxBufferLength);
  } else {
    // Interface recipient
    m_requestRecipients[1]->processSetupRequest(
        &m_request, m_largeBuffer, &m_transferBufferLength, maxBufferLength);
  }
}

void Endpoint0::processINpacket() {
  switch (m_state) {
    case State::DataIn:
      sendSomeData();
      break;
    case State::LastDataIn:
      m_state = State::StatusOut;
      // Prepare to receive the OUT Data[] transaction.
      setOutNAK(false);
      break;
    case State::StatusIn: {
      m_state = State::Idle;
      // All the data has been received. Callback the request recipient.
      uint8_t type = static_cast<uint8_t>(m_request.recipientType());
      if (type == 0) {
        // Device recipient
        m_requestRecipients[0]->wholeDataReceivedCallback(
            &m_request, m_largeBuffer, &m_transferBufferLength);
      } else {
        // Interface recipient
        m_requestRecipients[1]->wholeDataReceivedCallback(
            &m_request, m_largeBuffer, &m_transferBufferLength);
      }
    } break;
    default:
      stallTransaction();
  }
}

void Endpoint0::processOUTpacket() {
  switch (m_state) {
    case State::DataOut:
      if (receiveSomeData() < 0) {
        break;
      }
      if ((m_request.wLength() - m_transferBufferLength) <= k_maxPacketSize) {
        m_state = State::LastDataOut;
      }
      break;
    case State::LastDataOut:
      if (receiveSomeData() < 0) {
        break;
      }
      // Send the DATA1[] to the host.
      writePacket(NULL, 0);
      m_state = State::StatusIn;
      break;
    case State::StatusOut: {
      // Read the DATA1[] sent by the host.
      readPacket(NULL, 0);
      m_state = State::Idle;
      // All the data has been sent. Callback the request recipient.
      uint8_t type = static_cast<uint8_t>(m_request.recipientType());
      if (type == 0) {
        // Device recipient
        m_requestRecipients[0]->wholeDataSentCallback(&m_request, m_largeBuffer,
                                                      &m_transferBufferLength);
      } else {
        // Interface recipient
        m_requestRecipients[1]->wholeDataSentCallback(&m_request, m_largeBuffer,
                                                      &m_transferBufferLength);
      }
    } break;
    default:
      stallTransaction();
  }
}

void Endpoint0::flushTxFifo() {
  // Set IN endpoint NAK
  OTG.DIEPCTL0()->setSNAK(true);

  // Wait for core to respond
  // TODO: understand why !OTG.GOTGINT()->getSEDET() is required since verion 16
  while (!OTG.DIEPINT(0)->getINEPNE() && !OTG.GOTGINT()->getSEDET()) {
  }

  // Get the Tx FIFO number
  uint32_t fifo = OTG.DIEPCTL0()->getTXFNUM();

  // Wait for AHB idle
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  // Flush Tx FIFO
  OTG.GRSTCTL()->setTXFNUM(fifo);
  OTG.GRSTCTL()->setTXFFLSH(true);

  // Reset packet counter
  OTG.DIEPTSIZ0()->set(0);

  // Wait for the flush
  while (OTG.GRSTCTL()->getTXFFLSH()) {
  }
}

void Endpoint0::flushRxFifo() {
  // Set OUT endpoint NAK
  OTG.DOEPCTL0()->setSNAK(true);

  // Wait for AHB idle
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  // Flush Rx FIFO
  OTG.GRSTCTL()->setRXFFLSH(true);

  // Reset packet counter
  OTG.DOEPTSIZ0()->set(0);

  // Wait for the flush
  while (OTG.GRSTCTL()->getRXFFLSH()) {
  }
}

void Endpoint0::discardUnreadData() {
  for (int i = 0; i < m_receivedPacketSize; i += 4) {
    OTG.DFIFO0()->get();
  }
  m_receivedPacketSize = 0;
}

void Endpoint0::sendSomeData() {
  if (k_maxPacketSize < m_transferBufferLength) {
    // More than one packet needs to be sent
    writePacket(m_largeBuffer + m_bufferOffset, k_maxPacketSize);
    m_state = State::DataIn;
    m_bufferOffset += k_maxPacketSize;
    m_transferBufferLength -= k_maxPacketSize;
    return;
  }
  // Last data packet sent
  writePacket(m_largeBuffer + m_bufferOffset, m_transferBufferLength);
  if (m_zeroLengthPacketNeeded) {
    m_state = State::DataIn;
  } else {
    m_state = State::LastDataIn;
  }
  m_bufferOffset = 0;
  m_zeroLengthPacketNeeded = false;
  m_transferBufferLength = 0;
}

void Endpoint0::clearForOutTransactions(uint16_t wLength) {
  m_transferBufferLength = 0;
  m_state = (wLength > k_maxPacketSize) ? State::DataOut : State::LastDataOut;
  setOutNAK(false);
}

int Endpoint0::receiveSomeData() {
  // If it is the first chunk of data to be received, m_transferBufferLength is
  // 0.
  uint16_t packetSize =
      std::min(k_maxPacketSize, m_request.wLength() - m_transferBufferLength);
  uint16_t sizeOfPacketRead =
      readPacket(m_largeBuffer + m_transferBufferLength, packetSize);
  if (sizeOfPacketRead != packetSize) {
    stallTransaction();
    return -1;
  }
  m_transferBufferLength += packetSize;
  return packetSize;
}

uint16_t Endpoint0::readPacket(void *buffer, uint16_t length) {
  uint32_t *buffer32 = (uint32_t *)buffer;
  uint16_t buffer32Length = std::min(length, m_receivedPacketSize);

  int i;
  // The RX FIFO is read 4 bytes by 4 bytes
  for (i = buffer32Length; i >= 4; i -= 4) {
    *buffer32++ = OTG.DFIFO0()->get();
    m_receivedPacketSize -= 4;
  }

  if (i) {
    /* If there are remaining bytes that should be read, read the next 4 bytes
     * and copy only the wanted bytes. */
    uint32_t extraData = OTG.DFIFO0()->get();
    memcpy(buffer32, &extraData, i);
    if (m_receivedPacketSize < 4) {
      m_receivedPacketSize = 0;
    } else {
      m_receivedPacketSize -= 4;
    }
  }
  return buffer32Length;
}

uint16_t Endpoint0::writePacket(const void *buffer, uint16_t length) {
  const uint32_t *buffer32 = (uint32_t *)buffer;

  //  Return if there is already a packet waiting to be read in the TX FIFO
  if (OTG.DIEPTSIZ0()->getPKTCNT()) {
    return 0;
  }

  // Enable transmission

  class OTG::DIEPTSIZ0 dieptsiz0(0);
  // Indicate that the Transfer Size is one packet
  dieptsiz0.setPKTCNT(1);
  // Indicate the length of the Transfer Size
  dieptsiz0.setXFRSIZ(length);
  OTG.DIEPTSIZ0()->set(dieptsiz0);
  // Enable the endpoint
  OTG.DIEPCTL0()->setEPENA(true);
  // Clear the NAK bit
  OTG.DIEPCTL0()->setCNAK(true);

  // Copy the buffer to the TX FIFO by writing data 32bits by 32 bits.
  for (int i = length; i > 0; i -= 4) {
    OTG.DFIFO0()->set(*buffer32++);
  }

  return length;
}

void Endpoint0::stallTransaction() {
  OTG.DIEPCTL0()->setSTALL(true);
  m_state = State::Idle;
}

void Endpoint0::computeZeroLengthPacketNeeded() {
  if (m_transferBufferLength && m_transferBufferLength < m_request.wLength() &&
      m_transferBufferLength % k_maxPacketSize == 0) {
    m_zeroLengthPacketNeeded = true;
    return;
  }
  m_zeroLengthPacketNeeded = false;
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
