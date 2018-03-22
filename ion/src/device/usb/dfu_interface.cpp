#include "dfu_interface.h"
#include "../regs/flash.h"
#include <string.h>

namespace Ion {
namespace USB {
namespace Device {

static inline uint32_t min(uint32_t x, uint32_t y) { return (x<y ? x : y); }

//TODO vérifier qu'on ne change pas d'état si on est dans dfuError, sauf en cas de clear status

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
      return getStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t) DFURequest::ClearStatus:
      return clearStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t) DFURequest::Abort:
      return dfuAbort(transferBufferLength);
    case (uint8_t) DFURequest::GetState:
      return getState(transferBuffer, transferBufferLength, request->wValue());
    case (uint8_t) DFURequest::Download:
      return processDownloadRequest(request->wLength(), transferBufferLength);
    case (uint8_t) DFURequest::Upload:
      return processUploadRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
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
    if (m_largeBufferLength != 0) {
      // Here, copy the data from the transfer buffer to the flash memory
      writeOnMemory();
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
  if (request->bRequest() == (uint8_t) DFURequest::Download) {
    // Handle a download request
    if (request->wValue() == 0) {
      switch (transferBuffer[0]) {
        case (uint8_t) DFUDownloadCommand::SetAddressPointer:
          setAddressPointerCommand(request, transferBuffer, *transferBufferLength);
          return;
        case (uint8_t) DFUDownloadCommand::Erase:
          eraseCommand(transferBuffer, *transferBufferLength);
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
    // Compute the writing address
    m_writeAddress = (request->wValue() - 2) * k_maxTransferSize + m_addressPointer;
    // Store the received data unitl we copy it on the flash
    memcpy(m_largeBuffer, transferBuffer, *transferBufferLength);
    m_largeBufferLength = *transferBufferLength;
    m_state = State::dfuDNLOADSYNC;
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
    // Mass erase
    m_erasePage = k_flashMemorySectorsCount;
  } else {
    // Sector erase
    assert(transferBufferLength == 5);
    /* Find the sector number to erase. If the address is not a valid start of
     * sector, return an error. */
    uint32_t sectorAddresses[k_flashMemorySectorsCount] = {
      0x08000000,
      0x08004000,
      0x08008000,
      0x0800C000,
      0x08010000,
      0x08020000,
      0x08040000,
      0x08060000,
      0x08080000,
      0x080A0000,
      0x080C0000,
      0x080E0000
    };
    uint32_t eraseAddress = transferBuffer[1]
      + (transferBuffer[2] << 8)
      + (transferBuffer[3] << 16)
      + (transferBuffer[4] << 24);
    m_erasePage = k_flashMemorySectorsCount + 1;
    for (uint8_t i = 0; i < k_flashMemorySectorsCount; i++) {
      if (sectorAddresses[i] == eraseAddress) {
        m_erasePage = i;
        break;
      }
    }
    if (m_erasePage == k_flashMemorySectorsCount + 1) {
      m_state = State::dfuERROR;
      m_status = Status::errTARGET;
      return;
    }
  }
  m_state = State::dfuDNLOADSYNC;
  // The erase should be done after the next getStatus request.
}

void DFUInterface::unlockFlashMemory() {
  /* After a reset, program and erase operations are forbidden on the flash.
   * They can be unlocked by writting the appropriate keys in the FLASH_KEY
   * register. */
  FLASH.KEYR()->set(0x45670123);
  FLASH.KEYR()->set(0xCDEF89AB);
  // Set the parallelism size
  FLASH.CR()->setPSIZE(FLASH::CR::PSIZE::X32);
}

void DFUInterface::lockFlashMemory() {
  FLASH.CR()->setLOCK(1);
}

void DFUInterface::eraseMemoryIfNeeded() {
  if (m_erasePage == k_flashMemorySectorsCount + 1) {
    return;
  }
  // Unlock the Flash and check that no memory operation is ongoing
  unlockFlashMemory();
  while (FLASH.SR()->getBSY()) {
  }
  if (m_erasePage == k_flashMemorySectorsCount) {
    // Mass erase
    FLASH.CR()->setMER(true);
  } else {
    // Sector erase
    FLASH.CR()->setSER(true);
    FLASH.CR()->setSNB(m_erasePage);
  }
  // Trigger the erase operation
  FLASH.CR()->setSTRT(true);
  // Lock the Flash after all operations are done
  while (FLASH.SR()->getBSY()) {
  }
  lockFlashMemory();

  //TODO
  /*If a Flash memory write access concerns some data in the data cache, the Flash write access modifies the data in the Flash memory and the data in the cache.
   * If an erase operation in Flash memory also concerns data in the data or instruction cache, you have to make sure that these data are rewritten before they are accessed during code execution. If this cannot be done safely, it is recommended to flush the caches by setting the DCRST and ICRST bits in the FLASH_CR register.
   * The I/D cache should be flushed only when it is disabled (I/DCEN = 0).*/

  m_erasePage = k_flashMemorySectorsCount + 1; // Out of range value to indicate we do not need to erase anything.
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

void DFUInterface::writeOnMemory() {
  // TODO Check here the address is allowed, else return with dfuERROR and status errTARGET
  if (m_writeAddress >= 0x08000000 && m_writeAddress <= 0x080FA000) {
    //TODO get Flash adresses from linker script.
    // Check if the destination is the option bytes: it won't happen for us.
    // Unlock the Flash and check that no memory operation is ongoing
    unlockFlashMemory();
    while (FLASH.SR()->getBSY()) {
    }
    FLASH.CR()->setPG(true);

    uint32_t * source = reinterpret_cast<uint32_t *>(m_largeBuffer);
    uint32_t * destination = reinterpret_cast<uint32_t *>(m_writeAddress);
    for (int i=0; i<m_largeBufferLength/sizeof(uint32_t); i++) {
      *destination++ = *source++;
    }

    // Lock the Flash after all operations are done
    while (FLASH.SR()->getBSY()) {
    }
    lockFlashMemory();
  } else if (m_writeAddress >= 0x20000000 && m_writeAddress <= 0x2003E800) {
    //TODO get RAM adresses from linker script.
    // TODO We write in RAM, check we are not overriding the current instructions.
    memcpy((void *)m_writeAddress, m_largeBuffer, m_largeBufferLength);
  }
  // Reset the buffer length
  m_largeBufferLength = 0;
  // Change the interface state and status
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

bool DFUInterface::processUploadRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (m_state != State::dfuIDLE && m_state !=  State::dfuUPLOADIDLE) {
    m_ep0->stallTransaction();
    return false;
  }
  if (request->wValue() == 0) {
    // TODO Should we really do it anyway?
    /* The host requests to read the commands supported by the bootloader. After
     * receiving this command, the device returns N bytes representing the
     * command codes. The STM32 sends bytes as follows (N = 4):
     * Get command / Set Address Pointer / Erase / Read Unprotect */
  } else if (request->wValue() == 1) {
    m_ep0->stallTransaction();
    return false;
  } else {
    /* We decided to never protect Read operation. Else we would have to check
     * here it is not protected before reading. */

    // Compute the reading address
    uint32_t readAddress = (request->wValue() - 2) * k_maxTransferSize + m_addressPointer;
    // Copy the requested memory zone into the transfer buffer.
    uint16_t copySize = min(transferBufferMaxLength, request->wLength());
    memcpy(transferBuffer, (void *)readAddress, copySize);
    *transferBufferLength = copySize;
  }
  m_state = State::dfuUPLOADIDLE;
  return true;
}

}
}
}
