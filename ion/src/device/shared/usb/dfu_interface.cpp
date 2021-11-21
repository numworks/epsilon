
#include "dfu_interface.h"

#include <drivers/config/external_flash.h>
#include <drivers/config/internal_flash.h>
#include <drivers/external_flash.h>
#include <drivers/flash.h>
#include <drivers/internal_flash.h>
#include <ion/led.h>
#include <ion/storage.h>
#include <ion/timing.h>
#include <string.h>
namespace Ion {
namespace Device {
namespace USB {

static inline uint32_t minUint32T(uint32_t x, uint32_t y) { return x < y ? x : y; }

void DFUInterface::StatusData::push(Channel *c) const {
  c->push(m_bStatus);
  c->push(m_bwPollTimeout[2]);
  c->push(m_bwPollTimeout[1]);
  c->push(m_bwPollTimeout[0]);
  c->push(m_bState);
  c->push(m_iString);
}

void DFUInterface::StateData::push(Channel *c) const {
  c->push(m_bState);
}

void DFUInterface::wholeDataReceivedCallback(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength) {
  if (request->bRequest() == (uint8_t)DFURequest::Download) {
    // Handle a download request
    if (request->wValue() == 0) {
      // The request is a special command
      switch (transferBuffer[0]) {
        case (uint8_t)DFUDownloadCommand::SetAddressPointer:
          setAddressPointerCommand(request, transferBuffer, *transferBufferLength);
          return;
        case (uint8_t)DFUDownloadCommand::Erase:
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
    if (request->wLength() > 0) {
      // The request is a "real" download. Compute the writing address.
      m_writeAddress = (request->wValue() - 2) * Endpoint0::MaxTransferSize + m_addressPointer;

      // Store the received data until we copy it on the flash.
      memcpy(m_largeBuffer, transferBuffer, *transferBufferLength);
      m_largeBufferLength = *transferBufferLength;
      m_state = State::dfuDNLOADSYNC;
    }
  }
}

void DFUInterface::wholeDataSentCallback(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength) {
  if (request->bRequest() == (uint8_t)DFURequest::GetStatus) {
    // Do any needed action after the GetStatus request.
    if (m_state == State::dfuMANIFEST) {
      if (m_dfuLevel == 1 && m_isFirstExternalFlash && !m_isInternalLocked) {
        return;
      }
      /* If we leave the DFU and reset immediately, dfu-util outputs an error:
     * "File downloaded successfully
     *  dfu-util: Error during download get_status"
     * If we sleep 1us here, there is no error. We put 1ms for security.
     * This error might be due to the USB connection being cut too soon after
     * the last USB exchange, so the host does not have time to process the
     * answer received for the last GetStatus request. */
      Ion::Timing::msleep(1);
      // Leave DFU routine: Leave DFU, reset device, jump to application code
      leaveDFUAndReset();
    } else if (m_state == State::dfuDNBUSY) {
      m_state = State::dfuDNBUSY;
      if (m_largeBufferLength != 0) {
        // Here, copy the data from the transfer buffer to the flash memory
        writeOnMemory();
      }
      changeAddressPointerIfNeeded();
      eraseMemoryIfNeeded();
      m_state = State::dfuDNLOADIDLE;
    }
  }
}

bool DFUInterface::processSetupInRequest(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength, uint16_t transferBufferMaxLength) {
  if (Interface::processSetupInRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength)) {
    return true;
  }
  switch (request->bRequest()) {
    case (uint8_t)DFURequest::Detach:
      m_device->detach();
      return true;
    case (uint8_t)DFURequest::Download:
      return processDownloadRequest(request->wLength(), transferBufferLength);
    case (uint8_t)DFURequest::Upload:
      return processUploadRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t)DFURequest::GetStatus:
      return getStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t)DFURequest::ClearStatus:
      return clearStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t)DFURequest::GetState:
      return getState(transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (uint8_t)DFURequest::Abort:
      return dfuAbort(transferBufferLength);
  }
  return false;
}

bool DFUInterface::processDownloadRequest(uint16_t wLength, uint16_t *transferBufferLength) {
  if (m_state != State::dfuIDLE && m_state != State::dfuDNLOADIDLE) {
    m_state = State::dfuERROR;
    m_status = Status::errNOTDONE;
    m_ep0->stallTransaction();
    return false;
  }
  if (wLength == 0) {
    // Leave DFU routine: Reset the device and jump to application code
    m_state = State::dfuMANIFESTSYNC;
  } else {
    // Prepare to receive the download data
    m_ep0->clearForOutTransactions(wLength);
    m_state = State::dfuDNLOADSYNC;
  }
  return true;
}

bool DFUInterface::processUploadRequest(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength, uint16_t transferBufferMaxLength) {
  if (m_state != State::dfuIDLE && m_state != State::dfuUPLOADIDLE) {
    m_ep0->stallTransaction();
    return false;
  }
  if (request->wValue() == 0) {
    /* The host requests to read the commands supported by the bootloader. After
   * receiving this command, the device  should returns N bytes representing
   * the command codes for :
   * Get command / Set Address Pointer / Erase / Read Unprotect
   * We no not need it for now. */
    return false;
  } else if (request->wValue() == 1) {
    m_ep0->stallTransaction();
    return false;
  } else {
    /* We decided to never protect Read operation. Else we would have to check
   * here it is not protected before reading. */

    // Compute the reading address
    uint32_t readAddress = (request->wValue() - 2) * Endpoint0::MaxTransferSize + m_addressPointer;
    // Copy the requested memory zone into the transfer buffer.
    uint16_t copySize = minUint32T(transferBufferMaxLength, request->wLength());
    memcpy(transferBuffer, (void *)readAddress, copySize);
    *transferBufferLength = copySize;
  }
  m_state = State::dfuUPLOADIDLE;
  return true;
}

void DFUInterface::setAddressPointerCommand(SetupPacket *request, uint8_t *transferBuffer, uint16_t transferBufferLength) {
  assert(transferBufferLength == 5);
  // Compute the new address but change it after the next getStatus request.
  m_potentialNewAddressPointer = transferBuffer[1] + (transferBuffer[2] << 8) + (transferBuffer[3] << 16) + (transferBuffer[4] << 24);
  m_state = State::dfuDNLOADSYNC;
}

void DFUInterface::changeAddressPointerIfNeeded() {
  if (m_potentialNewAddressPointer == 0) {
    // There was no address change waiting.
    return;
  }
  // If there is a new address pointer waiting, change the pointer address.
  m_addressPointer = m_potentialNewAddressPointer;
  m_potentialNewAddressPointer = 0;
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

void DFUInterface::eraseCommand(uint8_t *transferBuffer, uint16_t transferBufferLength) {
  /* We determine whether the commands asks for a mass erase or which sector to
   * erase. The erase must be done after the next getStatus request. */
  m_state = State::dfuDNLOADSYNC;

  if (transferBufferLength == 1) {
    // Mass erase
    m_erasePage = Flash::TotalNumberOfSectors();
    return;
  }

  // Sector erase
  assert(transferBufferLength == 5);

  m_eraseAddress = transferBuffer[1] + (transferBuffer[2] << 8) + (transferBuffer[3] << 16) + (transferBuffer[4] << 24);

  m_erasePage = Flash::SectorAtAddress(m_eraseAddress);
  if (m_erasePage < 0) {
    // Unrecognized sector
    m_state = State::dfuERROR;
    m_status = Status::errTARGET;
  }
}

void DFUInterface::eraseMemoryIfNeeded() {
  if (m_erasePage < 0) {
    return;
  }
  willErase();
  if ((m_eraseAddress >= k_ExternalBorderAddress && m_eraseAddress < ExternalFlash::Config::EndAddress) || m_dfuUnlocked) {
    int32_t order = Flash::SectorAtAddress(m_eraseAddress);
    Flash::EraseSector(order);
  }
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
  m_erasePage = -1;
}

void DFUInterface::writeOnMemory() {
  if (m_writeAddress >= k_sramStartAddress && m_writeAddress <= k_sramEndAddress) {
    // Write on SRAM
    // FIXME We should check that we are not overriding the current instructions.
    memcpy((void *)m_writeAddress, m_largeBuffer, m_largeBufferLength);
    reset_custom_vars();  // On reset les vars car la ram n'a pas de secteur à effacer
  } else if (Flash::SectorAtAddress(m_writeAddress) >= 0) {
    if (m_dfuLevel == 2) {
      m_largeBufferLength = 0;
      m_state = State::dfuERROR;
      m_status = Status::errWRITE;
      return;
    }

    int current_memory_flashed;
    if (m_writeAddress >= InternalFlash::Config::StartAddress && m_writeAddress <= InternalFlash::Config::EndAddress) {
      if (m_isInternalLocked && !m_dfuUnlocked)  // On vérifie si l'external a été flash
      {
        m_largeBufferLength = 0;
        m_state = State::dfuERROR;
        m_status = Status::errTARGET;
        leaveDFUAndReset(false);  // Numworks flash l'internal avant donc on exit pour afficher le message
        return;
      }
      current_memory_flashed = 0;

      //on écrit dans la mémoire interne
      if (m_isFirstInternalFlash && !m_dfuUnlocked) {
        m_temp_is_valid = true;
        for (int i = 0; i < 4; i++) {
          if (magik[i] != m_largeBuffer[magik_adrs + i]) {
            m_temp_is_valid = false;
            break;
          }
        }
        if (!m_temp_is_valid) {
          m_largeBufferLength = 0;
          m_state = State::dfuERROR;
          m_status = Status::errVERIFY;
          //leaveDFUAndReset(); On ne leave plus sinon on fait crash la calc si il n'y a que la partie ext.
          return;
        } else {
          m_isFirstInternalFlash = false;
        }
      }
    } else {
      current_memory_flashed = 1;
      // Nous écrivons la partie external os
      if (m_writeAddress < k_ExternalBorderAddress && m_isFirstExternalFlash && !m_dfuUnlocked)  // On vérifie si on installe des apps ext
      {
        // if (m_dfuLevel == 1 && m_isInternalLocked) {
        //   m_largeBufferLength = 0;
        //   m_state = State::dfuERROR;
        //   m_status = Status::errTARGET;
        //   return;
        // }
        if (m_dfuLevel == 0) {
          // Partie moche du code parce que je n'arrivais pas à compil avec 3 boucles for sous msys
          int adress_magik = magik_ext_adrs[0];
          m_temp_is_valid = external_magik[0] == m_largeBuffer[adress_magik];
          m_largeBuffer[adress_magik] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[1] == m_largeBuffer[adress_magik + 1];
          m_largeBuffer[adress_magik + 1] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[2] == m_largeBuffer[adress_magik + 2];
          m_largeBuffer[adress_magik + 2] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[3] == m_largeBuffer[adress_magik + 3];
          m_largeBuffer[adress_magik + 3] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          adress_magik = magik_ext_adrs[1];

          m_temp_is_valid = external_magik[5] == m_largeBuffer[adress_magik];
          m_largeBuffer[adress_magik] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[6] == m_largeBuffer[adress_magik + 1];
          m_largeBuffer[adress_magik + 1] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[7] == m_largeBuffer[adress_magik + 2];
          m_largeBuffer[adress_magik + 2] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[8] == m_largeBuffer[adress_magik + 3];
          m_largeBuffer[adress_magik + 3] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          adress_magik = magik_ext_adrs[2];
          m_temp_is_valid = true;

          m_temp_is_valid = external_magik[0] == m_largeBuffer[adress_magik];
          m_largeBuffer[adress_magik] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[1] == m_largeBuffer[adress_magik + 1];
          m_largeBuffer[adress_magik + 1] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[2] == m_largeBuffer[adress_magik + 2];
          m_largeBuffer[adress_magik + 2] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[3] == m_largeBuffer[adress_magik + 3];
          m_largeBuffer[adress_magik + 3] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }

          m_temp_is_valid = external_magik[4] == m_largeBuffer[adress_magik + 4];
          m_largeBuffer[adress_magik + 4] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }
          m_temp_is_valid = external_magik[5] == m_largeBuffer[adress_magik + 5];
          m_largeBuffer[adress_magik + 5] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }
          m_temp_is_valid = external_magik[6] == m_largeBuffer[adress_magik + 6];
          m_largeBuffer[adress_magik + 6] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }
          m_temp_is_valid = external_magik[7] == m_largeBuffer[adress_magik + 7];
          m_largeBuffer[adress_magik + 7] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          }
          m_temp_is_valid = external_magik[8] == m_largeBuffer[adress_magik + 8];
          m_largeBuffer[adress_magik + 8] = 0xff;

          if (!m_temp_is_valid) {
            m_largeBufferLength = 0;
            leaveDFUAndReset(false);
            return;
          } else {
            m_isFirstExternalFlash = false;
            m_isInternalLocked = false;
          }
        } else {
          m_isFirstExternalFlash = false;
          m_isInternalLocked = false;
        }
      }
    }
    if (m_last_memoryFlashed >= 0 && current_memory_flashed != m_last_memoryFlashed) {
      m_last_memoryFlashed = -1;
    }

    m_erasePage = Flash::SectorAtAddress(m_writeAddress);

    //On vérifie qu'on a pas déjà effacé le secteur et si ce n'est pas un secteur external déjà effacé
    if ((m_last_memoryFlashed < 0 || m_erasePage != m_lastPageErased) && m_writeAddress < k_ExternalBorderAddress && !m_dfuUnlocked) {
      Flash::EraseSector(m_erasePage);
      m_last_memoryFlashed = current_memory_flashed;
    }

    m_lastPageErased = m_erasePage;
    m_erasePage = -1;

    Ion::Timing::msleep(1);
    Flash::WriteMemory(reinterpret_cast<uint8_t *>(m_writeAddress), m_largeBuffer, m_largeBufferLength);
  } else {
    // Invalid write address
    m_largeBufferLength = 0;
    m_state = State::dfuERROR;
    m_status = Status::errTARGET;
    return;
  }
  // Reset the buffer length
  m_largeBufferLength = 0;
  // Change the interface state and status
  m_state = State::dfuDNLOADIDLE;
  m_status = Status::OK;
}

bool DFUInterface::getStatus(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength, uint16_t transferBufferMaxLength) {
  // Change the status if needed
  if (m_state == State::dfuMANIFESTSYNC) {
    m_state = State::dfuMANIFEST;
  } else if (m_state == State::dfuDNLOADSYNC) {
    m_state = State::dfuDNBUSY;
  }
  // Copy the status on the TxFifo
  *transferBufferLength = StatusData(m_status, m_state).copy(transferBuffer, transferBufferMaxLength);
  return true;
}

bool DFUInterface::clearStatus(SetupPacket *request, uint8_t *transferBuffer, uint16_t *transferBufferLength, uint16_t transferBufferMaxLength) {
  m_status = Status::OK;
  m_state = State::dfuIDLE;
  return getStatus(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
}

bool DFUInterface::getState(uint8_t *transferBuffer, uint16_t *transferBufferLength, uint16_t maxSize) {
  *transferBufferLength = StateData(m_state).copy(transferBuffer, maxSize);
  return true;
}

bool DFUInterface::dfuAbort(uint16_t *transferBufferLength) {
  m_status = Status::OK;
  m_state = State::dfuIDLE;
  *transferBufferLength = 0;
  return true;
}

void DFUInterface::leaveDFUAndReset(bool do_reset) {
  reset_custom_vars();
  m_isInternalLocked = true;
  m_isFirstInternalFlash = true;
  m_isFirstExternalFlash = true;
  m_device->setResetOnDisconnect(do_reset);
  m_device->detach();
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
