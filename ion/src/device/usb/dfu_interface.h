#ifndef ION_DEVICE_USB_DFU_INTERFACE_H
#define ION_DEVICE_USB_DFU_INTERFACE_H

#include "interface.h"
#include "endpoint0.h"
#include "setup_packet.h"
#include <stddef.h>
#include <assert.h>

namespace Ion {
namespace USB {
namespace Device {

class DFUInterface : public Interface {

public:
  DFUInterface(Endpoint0 * ep0) :
    Interface(ep0),
    m_status(Status::OK),
    m_state(State::dfuIDLE),
    m_dataWaitingToBeFlashed(false),
    m_addressPointer(0),
    m_potentialNewAddressPointer(0),
    m_erasePage(0)
  {
  }
  void wholeDataReceivedCallback() override;

protected:
  void setActiveInterfaceAlternative(uint8_t interfaceAlternativeIndex) override {
    assert(interfaceAlternativeIndex == k_bInterfaceAlternativeValue);
  }
  uint8_t getActiveInterfaceAlternative() override {
    return k_bInterfaceAlternativeValue;
  }
  bool processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) override;
private:
  // DFU Request Codes
  enum class DFURequest {
    Download     = 1,
    Upload       = 2,
    GetStatus    = 3,
    ClearStatus  = 4,
    GetState     = 5,
    Abort        = 6
  };

  // DFU Download Commmand Codes
  enum class DFUDownloadCommand {
    GetCommand        = 0x00,
    SetAddressPointer = 0x21,
    Erase             = 0x41,
    ReadUnprotect     = 0x92
  };

  enum class Status : uint8_t {
    OK              = 0x00,
    errTARGET       = 0x01,
    errFILE         = 0x02,
    errWRITE        = 0x03,
    errERASE        = 0x04,
    errCHECK_ERASED = 0x05,
    errPROG         = 0x06,
    errVERIFY       = 0x07,
    errADDRESS      = 0x08,
    errNOTDONE      = 0x09,
    errFIRMWARE     = 0x0A,
    errVENDOR       = 0x0B,
    errUSBR         = 0x0C,
    errPOR          = 0x0D,
    errUNKNOWN      = 0x0E,
    errSTALLEDPKT   = 0x0F
  };

  enum class State : uint8_t {
    appIDLE             = 0,
    appDETACH           = 1,
    dfuIDLE             = 2,
    dfuDNLOADSYNC       = 3,
    dfuDNBUSY           = 4,
    dfuDNLOADIDLE       = 5,
    dfuMANIFESTSYNC     = 6,
    dfuMANIFEST         = 7,
    dfuMANIFESTWAITRESET = 8,
    dfuUPLOADIDLE       = 9,
    dfuERROR            = 10
  };

  class Data {
  public:
    uint16_t copy(void * target, size_t maxSize) const;
  };

  class StatusData : public Data {
  public:
    StatusData(Status status, uint32_t pollTimeout, State state) :
      m_bState((uint8_t)state),
      m_iString(0)
    {
      *((uint32_t *)&m_bStatus) = (((uint32_t)status << 24) | pollTimeout >> 8);
      /* m_bwPollTimeout should be 3 bytes, which is not a handy size. We cast
       * &m_bStatus to a 4-bytes pointer, fill in the first byte with the status
       * and the next three bytes with pollTimeout, which should be given in
       * microseconds and is then floored to the milliseconds value. */
    }
  private:
    uint8_t m_bStatus;  // Indication of the status resulting from the execution of the most recent request.
    uint8_t m_bwPollTimeout[3];
    uint8_t m_bState; // Indication of the state that the device is going to enter immediately following transmission of this response
    uint8_t m_iString;
  } __attribute__((packed));
  static_assert(sizeof(StatusData) == 6*sizeof(uint8_t));

  class StateData : public Data {
  public:
    StateData(State state) : m_bState((uint8_t)state) {}
  private:
    uint8_t m_bState; // Current state of the device
  };

  constexpr static uint8_t k_bInterfaceAlternativeValue = 0; // TODO bInterfaceNumber/bAlternateSetting from calculator.h. See https://www-user.tu-chemnitz.de/~heha/viewchm.php/hs/usb.chm/usb5.htm#AlternateSetting
  constexpr static uint32_t k_pollTimeout = 1000; // TODO: needed? value ?
  bool getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t maxSize);
  bool clearStatus(uint16_t wValue, uint8_t * transferBuffer, uint16_t * transferBufferLength);
  bool dfuAbort(uint16_t * transferBufferLength);
  bool getState(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t maxSize);
  bool processDownloadRequest(uint16_t wLength, uint16_t * transferBufferLength);
  bool processUploadRequest(uint16_t wValue, uint8_t * transferBuffer, uint16_t * transferBufferLength);
  void processWholeDataReceived(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength);
  void setAddressPointerCommand(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength);
  void changeAddressPointerIfNeeded();
  void eraseCommand(uint8_t * transferBuffer, uint16_t transferBufferLength);
  void eraseMemoryIfNeeded();
  void writeMemoryCommand(SetupPacket * request, uint8_t * transferBuffer, uint16_t transferBufferLength);

  Status m_status;
  State m_state;
  bool m_dataWaitingToBeFlashed;
  uint32_t m_addressPointer;
  uint32_t m_potentialNewAddressPointer;
  uint32_t m_erasePage;
};

}
}
}

#endif
