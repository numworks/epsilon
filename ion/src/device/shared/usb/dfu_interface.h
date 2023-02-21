#ifndef ION_DEVICE_SHARED_USB_DFU_INTERFACE_H
#define ION_DEVICE_SHARED_USB_DFU_INTERFACE_H

#include <assert.h>
#include <stddef.h>

#include "stack/device.h"
#include "stack/endpoint0.h"
#include "stack/interface.h"
#include "stack/setup_packet.h"
#include "stack/streamable.h"

namespace Ion {
namespace Device {
namespace USB {

class DFUInterface : public Interface {
 public:
  DFUInterface(Device* device, Endpoint0* ep0,
               uint8_t bInterfaceAlternateSetting)
      : Interface(ep0),
        m_device(device),
        m_status(Status::OK),
        m_state(State::dfuIDLE),
        m_addressPointer(0),
        m_potentialNewAddressPointer(k_nullAddress),
        m_erasePage(-1),
        m_largeBuffer{0},
        m_largeBufferLength(0),
        m_writeAddress(0),
        m_bInterfaceAlternateSetting(bInterfaceAlternateSetting),
        m_isErasingAndWriting(false) {}
  uint32_t addressPointer() const { return m_addressPointer; }
  void wholeDataReceivedCallback(SetupPacket* request, uint8_t* transferBuffer,
                                 uint16_t* transferBufferLength) override;
  void wholeDataSentCallback(SetupPacket* request, uint8_t* transferBuffer,
                             uint16_t* transferBufferLength) override;
  bool isErasingAndWriting() const { return m_isErasingAndWriting; }

 protected:
  void setActiveInterfaceAlternative(
      uint8_t interfaceAlternativeIndex) override {
    m_bInterfaceAlternateSetting = interfaceAlternativeIndex;
  }
  uint8_t getActiveInterfaceAlternative() override {
    return m_bInterfaceAlternateSetting;
  }
  bool processSetupInRequest(SetupPacket* request, uint8_t* transferBuffer,
                             uint16_t* transferBufferLength,
                             uint16_t transferBufferMaxLength) override;

 private:
  // DFU Request Codes
  enum class DFURequest {
    Detach = 0,
    Download = 1,
    Upload = 2,
    GetStatus = 3,
    ClearStatus = 4,
    GetState = 5,
    Abort = 6
  };

  // DFU Download Commmand Codes
  enum class DFUDownloadCommand {
    GetCommand = 0x00,
    SetAddressPointer = 0x21,
    Erase = 0x41,
    ReadUnprotect = 0x92
  };

  enum class Status : uint8_t {
    OK = 0x00,
    errTARGET = 0x01,
    errFILE = 0x02,
    errWRITE = 0x03,
    errERASE = 0x04,
    errCHECK_ERASED = 0x05,
    errPROG = 0x06,
    errVERIFY = 0x07,
    errADDRESS = 0x08,
    errNOTDONE = 0x09,
    errFIRMWARE = 0x0A,
    errVENDOR = 0x0B,
    errUSBR = 0x0C,
    errPOR = 0x0D,
    errUNKNOWN = 0x0E,
    errSTALLEDPKT = 0x0F
  };

  enum class State : uint8_t {
    appIDLE = 0,
    appDETACH = 1,
    dfuIDLE = 2,
    dfuDNLOADSYNC = 3,
    dfuDNBUSY = 4,
    dfuDNLOADIDLE = 5,
    dfuMANIFESTSYNC = 6,
    dfuMANIFEST = 7,
    dfuMANIFESTWAITRESET = 8,
    dfuUPLOADIDLE = 9,
    dfuERROR = 10
  };

  class StatusData : public Streamable {
   public:
    StatusData(Status status, State state, uint32_t pollTimeout = 1)
        : /* We put a default pollTimeout value of 1ms: if the device is busy,
           * the host has to wait 1ms before sending a getStatus Request. */
          m_bStatus((uint8_t)status),
          m_bwPollTimeout{uint8_t((pollTimeout >> 16) & 0xFF),
                          uint8_t((pollTimeout >> 8) & 0xFF),
                          uint8_t(pollTimeout & 0xFF)},
          m_bState((uint8_t)state),
          m_iString(0) {}

   protected:
    void push(Channel* c) const override;

   private:
    uint8_t m_bStatus;  // Status resulting from the execution of the most
                        // recent request
    uint8_t m_bwPollTimeout[3];  // m_bwPollTimeout is 24 bits
    uint8_t m_bState;  // State of the device immediately following transmission
                       // of this response
    uint8_t m_iString;
  };

  class StateData : public Streamable {
   public:
    StateData(State state) : m_bState((uint8_t)state) {}

   protected:
    void push(Channel* c) const override;

   private:
    uint8_t m_bState;  // Current state of the device
  };

  constexpr static uint32_t k_nullAddress = 0xFFFFFFFF;

  // Download and upload
  bool processDownloadRequest(uint16_t wLength, uint16_t* transferBufferLength);
  bool processUploadRequest(SetupPacket* request, uint8_t* transferBuffer,
                            uint16_t* transferBufferLength,
                            uint16_t transferBufferMaxLength);
  // Address pointer
  void setAddressPointerCommand(SetupPacket* request, uint8_t* transferBuffer,
                                uint16_t transferBufferLength);
  void changeAddressPointerIfNeeded();
  // Access memory
  void eraseCommand(uint8_t* transferBuffer, uint16_t transferBufferLength);
  void eraseMemoryIfNeeded();
  void writeOnMemory();
  void unlockFlashMemory();
  void lockFlashMemoryAndPurgeCaches();
  // Status
  bool getStatus(SetupPacket* request, uint8_t* transferBuffer,
                 uint16_t* transferBufferLength,
                 uint16_t transferBufferMaxLength);
  bool clearStatus(SetupPacket* request, uint8_t* transferBuffer,
                   uint16_t* transferBufferLength,
                   uint16_t transferBufferMaxLength);
  // State
  bool getState(uint8_t* transferBuffer, uint16_t* transferBufferLength,
                uint16_t maxSize);
  // Abort
  bool dfuAbort(uint16_t* transferBufferLength);
  // Leave DFU
  void leaveDFUAndReset();
  /* Erase and Write state. After starting the erase of flash memory, the user
   * can no longer leave DFU mode by pressing the Back key of the keyboard. This
   * way, we prevent the user from interrupting a software download. After every
   * software download, the calculator resets, which unlocks the "exit on
   * pressing back". */
  void willErase() { m_isErasingAndWriting = true; }

  Device* m_device;
  Status m_status;
  State m_state;
  uint32_t m_addressPointer;
  uint32_t m_potentialNewAddressPointer;
  int32_t m_erasePage;
  uint8_t m_largeBuffer[Endpoint0::MaxTransferSize];
  uint16_t m_largeBufferLength;
  uint32_t m_writeAddress;
  uint8_t m_bInterfaceAlternateSetting;
  bool m_isErasingAndWriting;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif
