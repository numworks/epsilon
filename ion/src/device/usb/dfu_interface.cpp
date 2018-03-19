#include "dfu_interface.h"
#include <ion.h> //TODO REMOVE
#include <kandinsky.h>//TODO REMOVE

namespace Ion {
namespace USB {
namespace Device {

//TODO vérifier qu'on ne change pas d'état si on est dans dfuError, sauf en cas de clear status

// DEBUG functions
void whiteScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorWhite);
}
void redScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorRed);
}
void blueScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorBlue);
}
void greenScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorGreen);
}
void yellowScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorYellow);
}
void blackScreen() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, 320,240), KDColorWhite);
}

void DFUInterface::StatusData::push(Channel * c) const {
  c->push(m_bStatus);
  c->push(m_bwPollTimeout[2]);
  c->push(m_bwPollTimeout[1]);
  c->push(m_bwPollTimeout[0]);
  c->push(m_bState);
  c->push(m_iString);
}

void DFUInterface::StateData::push(Channel * c) const {
  c->push(m_bState);
}

bool DFUInterface::processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (Interface::processSetupInRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength)) {
    return true;
  }
  switch (request->bRequest()) {
    case (uint8_t) DFURequest::GetStatus:
      whiteScreen();
      return getStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t) DFURequest::ClearStatus:
      greenScreen();
      return clearStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t) DFURequest::Abort:
      redScreen();
      return dfuAbort(transferBufferLength);
    case (uint8_t) DFURequest::GetState:
      blueScreen();
      return getState(transferBuffer, transferBufferLength, request->wValue());
    case (uint8_t) DFURequest::Download:
      yellowScreen();
      return processDownloadRequest(request->wLength(), transferBufferLength);
    case (uint8_t) DFURequest::Upload:
      blackScreen();
      return processUploadRequest(request->wValue(), transferBuffer, transferBufferLength);
  }
  return false;
}

bool DFUInterface::getStatus(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  bool actionsAfterStatus = false;
  // Change the status if needed
  if (m_state == State::dfuMANIFESTSYNC) {
    // TODO Here, go back to the code on the flash instead of the ram
    m_state = State::dfuIDLE;
  } else if (m_state == State::dfuDNLOADSYNC) {
    m_state = State::dfuDNBUSY;
    actionsAfterStatus = true;
  }
  // Copy the status on the TxFifo
  *transferBufferLength = StatusData(m_status, k_pollTimeout, m_state).copy(transferBuffer, transferBufferMaxLength);
  // Additional actions if needed
  if (actionsAfterStatus) {
    if (m_dataWaitingToBeFlashed) {
      // TODO Here, copy the data from the transfer buffer to the flash memory
      m_dataWaitingToBeFlashed = false;
    }
    changeAddressPointerIfNeeded();
    eraseMemoryIfNeeded();
    m_state = State::dfuDNLOADIDLE;
  }
  return true;
}

bool DFUInterface::clearStatus(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  m_status = Status::OK;
  m_state = State::dfuIDLE;
  return getStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
}

bool DFUInterface::dfuAbort(uint16_t * transferBufferLength) {
  m_status = Status::OK;
  m_state = State::dfuIDLE;
  *transferBufferLength = 0;
  return true;
}

bool DFUInterface::getState(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t maxSize) {
  *transferBufferLength = StateData(m_state).copy(transferBuffer, maxSize);
  return true;
}

bool DFUInterface::processDownloadRequest(uint16_t wLength, uint16_t * transferBufferLength) {
  if (m_state != State::dfuIDLE && m_state !=  State::dfuDNLOADIDLE) {
    m_ep0->stallTransaction();
    return false;
  }
  if (wLength == 0) {
    if (m_state == State::dfuIDLE ) {
      // If the device is idle, it should not receive a zero-length download.
      m_ep0->stallTransaction();
      return false;
    }
    // The download has ended, enter the manifestation phase.
    m_state = State::dfuMANIFESTSYNC;
  } else {
    // Prepare to receive the download data
    m_ep0->clearForOutTransactions(wLength);
    m_state = State::dfuDNLOADSYNC;
  }
  return true;
}

void DFUInterface::wholeDataReceivedCallback(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength) {
  m_state = State::dfuDNLOADSYNC;
  processWholeDataReceived(request, transferBuffer, *transferBufferLength);
}

void DFUInterface::processWholeDataReceived(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength) {
  if (request->bRequest() == (uint8_t) DFURequest::Download) {
    // Handle a download request
    if (request->wValue() == 0) {
      switch (transferBuffer[0]) {
        case (uint8_t) DFUDownloadCommand::SetAddressPointer:
          setAddressPointerCommand(request, transferBuffer, transferBufferLength);
          return;
        case (uint8_t) DFUDownloadCommand::Erase:
          eraseCommand(transferBuffer, transferBufferLength);
          return;
        default:
          m_state = State::dfuERROR;
          m_status = Status::errSTALLEDPKT;
          return;
      }
    }
    if (request->wValue() == 1) {
      m_ep0->stallTransaction();
      return;
    }
    writeMemoryCommand(request, transferBuffer, transferBufferLength);
  }
}

void DFUInterface::setAddressPointerCommand(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength) {
  assert(transferBufferLength == 5);
  m_potentialNewAddressPointer = transferBuffer[1]
    + (transferBuffer[2] << 8)
    + (transferBuffer[3] << 16)
    + (transferBuffer[4] << 24);
  // TODO Check the address is allowed.
  m_state = State::dfuDNLOADSYNC;
  // The address change should be done after the next getStatus request.
}

void DFUInterface::changeAddressPointerIfNeeded() {
  if (m_potentialNewAddressPointer == 0) {
    return;
  }
  // If there is a new address pointer waiting, change the pointer address.
  m_addressPointer = m_potentialNewAddressPointer;
  m_potentialNewAddressPointer = 0;
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

void DFUInterface::eraseCommand(uint8_t * transferBuffer, uint16_t transferBufferLength) {
  if (transferBufferLength == 1) {
    // MASS ERASE
    m_erasePage = 1; //TODO Make sure this is not a valid address
  } else {
    assert(transferBufferLength == 5);
    m_erasePage = transferBuffer[1]
      + (transferBuffer[2] << 8)
      + (transferBuffer[3] << 16)
      + (transferBuffer[3] << 24);
  }
  m_state = State::dfuDNLOADSYNC;
  // The erase should be done after the next getStatus request.
}

void DFUInterface::eraseMemoryIfNeeded() {
  if (m_erasePage == 0) {
    return;
  }
  if (m_erasePage == 1) {
    //TODO Mass erase.
  } else {
    //TODO Check the address is allowed. Return error if not.
    //TODO Erase the right page of memory.
  }
  m_erasePage = 0;
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

void DFUInterface::writeMemoryCommand(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength) {
  // Compute the writing address
  uint32_t writeAddress = (request->wValue() - 2) * request->wLength() + m_addressPointer;
  // TODO comment why we won't do Check it is allowed (if not, dfuERROR and status errTARGET)
  // TODO comment why we won't doCheck the ROP is not active (???)

  // Write the received buffer to the destination address
  //TODO
  // Check if the destination is the option bytes (Won't happen)
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

bool DFUInterface::processUploadRequest(uint16_t wValue, uint8_t * transferBuffer, uint16_t * transferBufferLength) {
  if (m_state != State::dfuIDLE && m_state !=  State::dfuUPLOADIDLE) {
    m_ep0->stallTransaction();
    return false;
  }
  // TODO (ne pas faire, stall ?) Si l'adresse du pointeur de flash est 0, envoyer les commandes supportées
  // TODO Si 1, stall
  // TODO Read Operation Protected ?? Si oui, envoyer dfuERROR

  // TODO Copier les données dans le transfer buffer.
  m_state = State::dfuUPLOADIDLE;
  return true;
}

}
}
}
