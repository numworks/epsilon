#include "endpoint0.h"
#include "device.h"
#include "interface.h"
#include "request_recipient.h"
#include "../regs/regs.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace Ion {
namespace USB {
namespace Device {

void Endpoint0::setup() {
  /* Setup the IN direction */

  // Reset the device IN endpoint 0 control register
  OTG.DIEPCTL0()->set(0);
  // Reset the device IN endpoint 0 transfer size register
  OTG.DIEPTSIZ0()->set(0);
  // Maximum packet size
  OTG.DIEPCTL0()->setMPSIZ(OTG::DIEPCTL0::MPSIZ::Size64);
  // Transfer size. The core interrupts the application only after it has
  // exhausted the transfer size amount of data. The transfer size is set to the
  // maximum packet size, to be interrupted at the end of each packet.
  OTG.DIEPTSIZ0()->setXFRSIZ(k_maxPacketSize);
  // Set the NAK bit: all IN transactions on endpoint 0 receive a NAK answer
  OTG.DIEPCTL0()->setSNAK(true);
  // Enable the endpoint
  OTG.DIEPCTL0()->setEPENA(true);

  /* Setup the OUT direction */
  setupOut();
  // Set the NAK bit
  OTG.DOEPCTL0()->setSNAK(true);
  // Enable the endpoint
  enableOut();

  /* Setup the Tx FIFO */
  // Tx FIFO depth. Division by 4 because the value is in terms of 32-bit words.
  OTG.DIEPTXF0()->setTX0FD(k_maxPacketSize/4);
  // Tx RAM start address. It starts just after the Rx FIFO. //TODO: get the value from elsewhere.
  OTG.DIEPTXF0()->setTX0FSA(128);
}

void Endpoint0::setupOut() {
  class OTG::DOEPTSIZ0 doeptsiz0(0);
  // Number of back-to-back SETUP data packets the endpoint can receive
  doeptsiz0.setSTUPCNT(1);
  // Packet count, false if a packet is written into the Rx FIFO
  doeptsiz0.setPKTCNT(true);
  // Transfer size. The core interrupts the application only after it has
  // exhausted the transfer size amount of data. The transfer size is set to the
  // maximum packet size, to be interrupted at the end of each packet.
  doeptsiz0.setXFRSIZ(64);
  OTG.DOEPTSIZ0()->set(doeptsiz0);
}

void Endpoint0::setOutNAK(bool nak) {
  m_forceNAK = nak;
  if (nak) {
    OTG.DOEPCTL0()->setSNAK(true);
    return;
  }
  OTG.DOEPCTL0()->setCNAK(true);
}

void Endpoint0::enableOut() {
  OTG.DOEPCTL0()->setEPENA(true);
}

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
  uint16_t maxBufferLength = MIN(m_request.wLength(), k_largeBufferLength);

  // Requests are only sent to the device or the interface for now.
  assert(((uint8_t)m_request.recipientType() == 0) || ((uint8_t)m_request.recipientType() == 1));
  m_requestRecipients[(uint8_t)(m_request.recipientType())]->processSetupRequest(&m_request, m_largeBuffer, &m_transferBufferLength, maxBufferLength);
}

void Endpoint0::processINpacket() {
  switch (m_state) {
    case State::DataIn:
      sendSomeData();
      break;
    case State::LastDataIn:
      m_state = State::StatusOut;
      setOutNAK(false); // TODO Why not In?
      break;
    case State::StatusIn:
      m_state = State::Idle;
      // All the data has been received. Callback the request recipient.
      m_requestRecipients[(uint8_t)(m_request.recipientType())]->wholeDataReceivedCallback(&m_request, m_largeBuffer, &m_transferBufferLength);
      break;
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
      writePacket(NULL, 0); // Send the DATA1[] to the host.
      m_state = State::StatusIn;
      break;
    case State::StatusOut:
      readPacket(NULL, 0); // Read the DATA1[] sent by the host.
      m_state = State::Idle;
      break;
    default:
      stallTransaction();
  }
}

void Endpoint0::flushTxFifo() {
  // Set IN endpoint NAK
  OTG.DIEPCTL0()->setSNAK(true);

  // Wait for core to respond
  while (!OTG.DIEPINT(0)->getINEPNE()) {
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
  // Set the transfer state.
  m_state = (wLength > k_maxPacketSize) ? State::DataOut : State::LastDataOut;
  setOutNAK(false);
}

uint16_t Endpoint0::receiveSomeData() {
  // If it is the first chunk of data to be received, m_transferBufferLength is 0.
  uint16_t packetSize = MIN(k_maxPacketSize, m_request.wLength() - m_transferBufferLength);
  uint16_t sizeOfPacketRead = readPacket(m_largeBuffer + m_transferBufferLength, packetSize);
  if (sizeOfPacketRead != packetSize) {
    stallTransaction();
    return -1;
  }
  m_transferBufferLength += packetSize;
  return packetSize;
}

uint16_t Endpoint0::readPacket(void * buffer, uint16_t length) {
  uint32_t * buffer32 = (uint32_t *) buffer;
  uint16_t buffer32Length = MIN(length, m_receivedPacketSize);

  int i;
  // The RX FIFO is read 4 bytes by 4 bytes
  for (i = buffer32Length; i >= 4; i -= 4) {
    *buffer32++ = OTG.DFIFO0()->get();
    m_receivedPacketSize -= 4;
  }

  if (i) {
    // If there are remaining bytes that should be read, read the next 4 bytes
    // and copy only the wanted bytes.
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

uint16_t Endpoint0::writePacket(const void * buffer, uint16_t length) {
  const uint32_t * buffer32 = (uint32_t *) buffer;

  /*  Return if there is already a packet waiting to be read in the TX FIFO */
  if (OTG.DIEPTSIZ0()->getPKTCNT()) {
    return 0;
  }

  /* Enable transmission */
  // Reset the device IN endpoint 0 transfer size register
  OTG.DIEPTSIZ0()->set(0);
  // Indicate that the Transfer Size is one packet
  OTG.DIEPTSIZ0()->setPKTCNT(1);
  // Indicate the length of the Transfer Size
  OTG.DIEPTSIZ0()->setXFRSIZ(length);
  // Enable the endpoint
  OTG.DIEPCTL0()->setEPENA(true);
  // Remove the NAK bit
  OTG.DIEPCTL0()->setCNAK(true);

  /* Copy the buffer to the TX FIFO */
  // memcpy does not work  //TODO Why?
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
  if (m_transferBufferLength
    && m_transferBufferLength < m_request.wLength()
    && m_transferBufferLength % k_maxPacketSize == 0)
  {
    m_zeroLengthPacketNeeded = true;
    return;
  }
  m_zeroLengthPacketNeeded = false;
}

}
}
}
