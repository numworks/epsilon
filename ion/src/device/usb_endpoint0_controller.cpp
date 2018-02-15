#include "usb_endpoint0_controller.h"
#include "usb_device.h"
#include "usb_control_transfer_controller.h"
#include "regs/regs.h"

namespace Ion {
namespace USB {
namespace Device {

Endpoint0Controller::Endpoint0Controller() :
  m_forceNAK(false)
{
}

void Endpoint0Controller::setup() {
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
  enableOUT(true);

  /* Setup the Tx FIFO */
  // Tx FIFO depth. Division by 4 because the value is in terms of 32-bit words.
  OTG.DIEPTXF0()->setTX0FD(k_maxPacketSize/4);
  // Tx RAM start address. It starts just after the Rx FIFO. //TODO: get the value from elsewhere.
  OTG.DIEPTXF0()->setTX0FSA(128);
}

void Endpoint0Controller::setupOut() {
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

void Endpoint0Controller::reset() {
  flushTxFifo();
  flushRxFifo();
}

void Endpoint0Controller::enableOUT(bool shouldEnable) {
  OTG.DOEPCTL0()->setEPENA(shouldEnable);
}

void Endpoint0Controller::sendChunk(USBDevice * device) {
  if (k_maxPacketSize < device->controlTransferController()->controlTransferBufferLength()) {
    // More than one packet needs to be sent
    writePacket(device, device->controlTransferController()->controlTransferBuffer(), k_maxPacketSize);
    device->controlTransferController()->setState(ControlTransferController::State::DataIn);
    device->controlTransferController()->setControlTransferBuffer(device->controlTransferController()->controlTransferBuffer() + k_maxPacketSize);
    device->controlTransferController()->setControlTransferBufferLength(device->controlTransferController()->controlTransferBufferLength() - k_maxPacketSize);
  } else {
    // Last data packet sent
    writePacket(device, device->controlTransferController()->controlTransferBuffer(), device->controlTransferController()->controlTransferBufferLength());
    if (device->controlTransferController()->zeroLengthPacketNeeded()) {
      device->controlTransferController()->setState(ControlTransferController::State::DataIn);
    } else {
      device->controlTransferController()->setState(ControlTransferController::State::LastDataIn);
    }
    device->controlTransferController()->setZeroLengthPacketNeeded(false);
    device->controlTransferController()->setControlTransferBuffer(NULL);
    device->controlTransferController()->setControlTransferBufferLength(0);
  }
}

int Endpoint0Controller::receiveChunk(USBDevice * device) {
  uint16_t packetSize = MIN(k_maxPacketSize, device->controlTransferController()->setupData().wLength - device->controlTransferController()->controlTransferBufferLength());
  uint16_t sizeOfPacketRead = readPacket(device, device->controlTransferController()->controlTransferBuffer() + device->controlTransferController()->controlTransferBufferLength(), packetSize);
  if (sizeOfPacketRead != packetSize) {
    stallTransaction(device);
    return -1;
  }
  device->controlTransferController()->setControlTransferBufferLength(device->controlTransferController()->controlTransferBufferLength() + packetSize);
  return packetSize;
}

void Endpoint0Controller::stallTransaction(USBDevice * device) {
  OTG.DIEPCTL0()->setSTALL(true);
  device->controlTransferController()->setState(ControlTransferController::State::Idle);
}

void Endpoint0Controller::setOutNAK(bool nak) {
  m_forceNAK = nak;
  if (nak) {
    OTG.DOEPCTL0()->setSNAK(true);
    return;
  }
  OTG.DOEPCTL0()->setCNAK(true);
}

void Endpoint0Controller::flushTxFifo() {
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

void Endpoint0Controller::flushRxFifo() {
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

uint16_t Endpoint0Controller::readPacket(USBDevice * device, void * buffer, uint16_t length) {
  uint32_t * buffer32 = (uint32_t *) buffer;
  uint16_t lengthBuffer32 = MIN(length, device->receivedPacketSize());

  int i;
  // The RX FIFO is read 4 bytes by 4 bytes
  for (i = lengthBuffer32; i >= 4; i -= 4) {
    *buffer32++ = OTG.DFIFO0()->get();
    device->setReceivedPacketSize(device->receivedPacketSize() - 4);
  }

  if (i) {
    // If there are remaining bytes that should be read, read the next 4 bytes
    // and copy only the wanted bytes.
    uint32_t extraData = OTG.DFIFO0()->get();
    memcpy(buffer32, &extraData, i);
    if (device->receivedPacketSize() < 4) {
      device->setReceivedPacketSize(0);
    } else {
      device->setReceivedPacketSize(device->receivedPacketSize() - 4);
    }
  }
  return lengthBuffer32;
}

uint16_t Endpoint0Controller::writePacket(USBDevice * device, const void * buffer, uint16_t length) {
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

}
}
}
