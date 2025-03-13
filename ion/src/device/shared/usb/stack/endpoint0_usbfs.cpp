#include <assert.h>
#include <scandium/shared/drivers/usb.h>
#include <stm32u083/usb.h>
#include <stm32u083/usb_sram.h>
#include <string.h>

#include <algorithm>

#include "device.h"
#include "endpoint0.h"
#include "helpers_usbfs.h"
#include "interface.h"
#include "request_recipient.h"

namespace Ion {
namespace Device {
namespace USB {

using STM32U083::USB_SRAM;
using USBFS = STM32U083::USB;

void Endpoint0::setup() {}

void Endpoint0::setupOut() {}

void Endpoint0::enableOut() {
#if 0
  OTG.DOEPCTL0()->setEPENA(true);
#endif
}

void Endpoint0::reset() {
  flushTxFifo();
  flushRxFifo();
}

void Endpoint0::readAndDispatchSetupPacket() {
  CHEP0R::SetSTATRX(Status::NAK);
  // CHEP0R::SetSTATTX(Status::NAK);
  assert(CHEP0R::GetSTATRX() == Status::NAK);
  // setNAK(true);

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
    case State::LastDataIn: {
      m_state = State::StatusOut;
      // Prepare to receive the OUT Data[] transaction.
      // setNAK(false);
      CHEP0R::SetSTATRX(Status::NAK);
      CHEP0R::SetEPKIND(true);  // STATUS_OUT
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
      CHEP0R::SetSTATRX(Status::Valid);
      break;
    }
    case State::StatusIn: {
      m_state = State::Idle;
      // All the data has been received. Callback the request recipient.
      CHEP0R::SetSTATRX(Status::Valid);
      CHEP0R::SetSTATTX(Status::NAK);
      break;
    }
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
        CHEP0R::SetSTATTX(Status::NAK);
      } else {
        CHEP0R::SetSTATTX(Status::Stall);
      }
      CHEP0R::SetSTATRX(Status::Valid);
      break;
    case State::LastDataOut: {
      if (receiveSomeData() < 0) {
        break;
      }
      CHEP0R::SetSTATTX(Status::NAK);
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
      // Send the DATA1[] to the host.
      writePacket(NULL, 0);
      CHEP0R::SetSTATTX(Status::Valid);
      m_state = State::StatusIn;
      break;
    }
    case State::StatusOut: {
      // Read the DATA1[] sent by the host.
      readPacket(NULL, 0);
      m_state = State::Idle;
      CHEP0R::SetEPKIND(false);
      CHEP0R::SetSTATRX(Status::Valid);
      CHEP0R::SetSTATTX(Status::NAK);
    } break;
    default:
      stallTransaction();
  }
}

void Endpoint0::flushTxFifo() {
  // CHEP0R::SetSTATTX(Status::NAK); ?
}

void Endpoint0::flushRxFifo() {
  // CHEP0R::SetSTATRX(Status::NAK); ?
}

void Endpoint0::discardUnreadData() { m_receivedPacketSize = 0; }

void Endpoint0::sendSomeData() {
  if (k_maxPacketSize < m_transferBufferLength) {
    // More than one packet needs to be sent
    writePacket(m_largeBuffer + m_bufferOffset, k_maxPacketSize);
    m_state = State::DataIn;
    CHEP0R::SetSTATRX(Status::Stall);
    CHEP0R::SetSTATTX(Status::Valid);
    m_bufferOffset += k_maxPacketSize;
    m_transferBufferLength -= k_maxPacketSize;
    return;
  }
  // Last data packet sent
  writePacket(m_largeBuffer + m_bufferOffset, m_transferBufferLength);
  if (m_zeroLengthPacketNeeded) {
    m_state = State::DataIn;
    CHEP0R::SetSTATRX(Status::Stall);
  } else {
    m_state = State::LastDataIn;
    CHEP0R::SetSTATRX(Status::NAK);
  }
  CHEP0R::SetSTATTX(Status::Valid);
  m_bufferOffset = 0;
  m_zeroLengthPacketNeeded = false;
  m_transferBufferLength = 0;
}

void Endpoint0::clearForOutTransactions(uint16_t wLength) {
  m_transferBufferLength = 0;
  m_state = (wLength > k_maxPacketSize) ? State::DataOut : State::LastDataOut;
  assert(CHEP0R::GetSTATRX() == Status::NAK);
  CHEP0R::SetSTATTX(Status::Stall);
  CHEP0R::SetSTATRX(Status::Valid);
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

uint16_t Endpoint0::readPacket(void* buffer, uint16_t length) {
  uint32_t* buffer32 = (uint32_t*)buffer;
  uint16_t buffer32Length = std::min(length, m_receivedPacketSize);

  int received = USB_SRAM::CHEP_RXTXBD_0::Read().getCOUNT_RX();
  assert(received <= k_maxPacketSize);
  memcpy(buffer32, reinterpret_cast<char*>(Hal::Usb::k_rxBufferAddr), received);
  m_receivedPacketSize = std::max(0, m_receivedPacketSize - received);

  assert(CHEP0R::GetSTATRX() != Status::Valid);

  /* TODO: We need to wait to have read the packet before clearing vtrx since a
   * setup packet can be copied to the RX buffer even without the STATRX
   * Valid. */
  // clearVTRX();
  return buffer32Length;
}

uint16_t Endpoint0::writePacket(const void* buffer, uint16_t length) {
  if (CHEP0R::GetSTATTX() == Status::Valid) {
    // Return if there is already a packet waiting to be read in the TX FIFO
    // TODO: how can this happen ?
    return 0;
  }

  assert(length < Hal::Usb::k_txBufferLen);
  if (length) {
    // We cannot use memcpy this because we need the access to be 4 by 4
    for (int i = 0; i < (length + 3) / 4; i++) {
      // FIXME we may read pass buffer
      reinterpret_cast<uint32_t*>(Hal::Usb::k_txBufferAddr)[i] =
          reinterpret_cast<const uint32_t*>(buffer)[i];
    }
  }
  USB_SRAM::CHEP_TXRXBD_0::Read().setCOUNT_TX(length).write();
  asm volatile("dmb 0xF" ::: "memory");
  return length;
}

void Endpoint0::stallTransaction() {
  // TODO: the stall may be cleared by a setNAK afterwards
  CHEP0R::SetSTATRX(Status::Stall);
  CHEP0R::SetSTATTX(Status::Stall);
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
