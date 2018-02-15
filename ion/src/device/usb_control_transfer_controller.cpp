#include "usb_control_transfer_controller.h"
#include "usb_device.h"
#include "usb_endpoint0_controller.h"
#include "usb_data.h"

namespace Ion {
namespace USB {
namespace Device {

static const struct DeviceDescriptor sDeviceDescriptor = {
  .bLength = USB_DT_DEVICE_SIZE,
  .bDescriptorType = USB_DT_DEVICE,
  .bcdUSB = 0x0300,
  .bDeviceClass = 0,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64, // TODO merge this 64 with Endpoint0Controller::k_maxPacketSize
  .idVendor = 0xfeca, //TODO Buy one!
  .idProduct = 0xfeca, //TODO Create one!
  .bcdDevice = 0x0001,
  .iManufacturer = 1,
  .iProduct = 2,
  .iSerialNumber = 3,
  .bNumConfigurations = 1
};

static const struct ConfigDescriptor sConfigDescriptor = {
  .bLength = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType = USB_DT_CONFIGURATION,
  .wTotalLength = 0, // The value is set in buildConfigDescriptor
  .bNumInterfaces = 1,
  .bConfigurationValue = USB_DFU_CONFIGURATION_VALUE,
  .iConfiguration = 0,
  .bmAttributes = 0x80,
  .bMaxPower = 0x32
};

static const struct InterfaceDescriptor sInterfaceDescriptor = {
  .bLength = USB_DT_INTERFACE_SIZE,
  .bDescriptorType = USB_DT_INTERFACE,
  .bInterfaceNumber = 0,
  .bAlternateSetting = 0,
  .bNumEndpoints = 0,
  .bInterfaceClass = USB_CLASS_APPLICATION_SPECIFIC,
  .bInterfaceSubClass = USB_SUBCLASS_DFU,
  .bInterfaceProtocol = USB_PROTOCOL_DFU,
  .iInterface = 0
};

ControlTransferController::ControlTransferController(USBDevice * usbDevice, Endpoint0Controller * endpoint0Controller) :
  m_state(State::Idle),
  m_usbDevice(usbDevice),
  m_endpoint0Controller(endpoint0Controller),
  m_receivedPacketSize(0),
  m_transferBuffer(nullptr),
  m_transferBufferLength(0),
  m_zeroLengthPacketNeeded(false)
{
}

void ControlTransferController::didFinishControlSetupTransaction() {
  m_endpoint0Controller->setOutNAK(true); // TODO: Why not for IN?

  // Read the 8-bytes Setup packet
  if (m_endpoint0Controller->readPacket(m_usbDevice, &m_setupData, k_setupDataLength) != k_setupDataLength) {
    m_endpoint0Controller->stallTransaction(m_usbDevice);
    return;
  };

  if (m_setupData.wLength == 0) {
    didFinishControlSetupBeforeInTransaction();
  } else if (SetupDataHelper::bmRequestTypeDirection(m_setupData.bmRequestType) == DataDirection::In) {
    didFinishControlSetupBeforeInTransaction();
  } else {
    assert(SetupDataHelper::bmRequestTypeDirection(m_setupData.bmRequestType) == DataDirection::Out);
    didFinishControlSetupBeforeOutTransaction();
  }
}

void ControlTransferController::didFinishControlInTransaction() {
  switch (m_state) {
    case State::DataIn:
      m_endpoint0Controller->sendChunk(m_usbDevice);
      break;
    case State::LastDataIn:
      m_state = State::StatusOut;
      m_endpoint0Controller->setOutNAK(false); // TODO Why not In?
      break;
    case State::StatusIn:
      m_state = State::Idle;
      break;
    default:
      m_endpoint0Controller->stallTransaction(m_usbDevice);
  }
}

void ControlTransferController::didFinishControlOutTransaction() {
  switch (m_state) {
    case State::DataOut:
      if (m_endpoint0Controller->receiveChunk(m_usbDevice) < 0) {
        break;
      }
      if ((m_setupData.wLength - m_transferBufferLength) <= Endpoint0Controller::k_maxPacketSize) {
        m_state = State::LastDataOut;
      }
      break;
    case State::LastDataOut:
      if (m_endpoint0Controller->receiveChunk(m_usbDevice) < 0) {
        break;
      }
      /*
       * We have now received the full data payload.
       * Invoke callback to process.
       */
      if (controlRequestDispatch()) {
        /* Go to status stage on success. */
        m_endpoint0Controller->writePacket(m_usbDevice, NULL, 0); // Send the DATA1[] to the host.
        m_state = State::StatusIn;
      } else {
        m_endpoint0Controller->stallTransaction(m_usbDevice);
      }
      break;
    case State::StatusOut:
      m_endpoint0Controller->readPacket(m_usbDevice, NULL, 0); // Read the DATA1[] sent by the host.
      m_state = State::Idle;
      break;
    default:
      m_endpoint0Controller->stallTransaction(m_usbDevice);
  }
}

void ControlTransferController::setControlTransferBuffer(uint8_t * buffer) {
  m_transferBuffer = buffer;
}

void ControlTransferController::setControlTransferBufferLength(uint16_t length) {
  m_transferBufferLength = length;
}

void ControlTransferController::setState(State state) {
  m_state = state;
}

bool ControlTransferController::computeZeroLengthPacketNeeded(uint16_t dataLength, uint16_t dataExpectedLength, uint8_t endpointMaxPacketSize) {
  if (dataLength < dataExpectedLength) {
    if (dataLength && dataLength % endpointMaxPacketSize == 0) {
      /* TODO I do not think this condition is needed: dataLength */
      return true;
    }
  }
  return false;
}

void ControlTransferController::setZeroLengthPacketNeeded(bool isNeeded) {
  m_zeroLengthPacketNeeded = isNeeded;
}

void ControlTransferController::setReceivedPacketSize(uint16_t size) {
  m_receivedPacketSize = size;
}

void ControlTransferController::didFinishControlSetupBeforeInTransaction() {
  m_transferBuffer = m_transferBufferInit;
  m_transferBufferLength = m_setupData.wLength;

  if (controlRequestDispatch()) {

    if (m_setupData.wLength) {
      // The host is waiting for device data. Check if we need to send a Zero
      // Length Packet to explicit a short transaction.
      m_zeroLengthPacketNeeded = computeZeroLengthPacketNeeded(m_transferBufferLength, m_setupData.wLength, Endpoint0Controller::k_maxPacketSize);
      // Send the data.
      m_endpoint0Controller->sendChunk(m_usbDevice);
    } else {
      // If no data is expected, send a zero length packet
      m_endpoint0Controller->writePacket(m_usbDevice, NULL, 0);
      m_state = State::StatusIn;
    }
  } else {
    // Stall endpoint on failure
    m_endpoint0Controller->stallTransaction(m_usbDevice);
  }
}

void ControlTransferController::didFinishControlSetupBeforeOutTransaction() {
  if (m_setupData.wLength > k_transferBufferInitLength) {
    m_endpoint0Controller->stallTransaction(m_usbDevice);
    return;
  }

  // Buffer into which to write received data
  m_transferBuffer = m_transferBufferInit;
  m_transferBufferLength = 0;
  // Wait for DATA OUT stage.
  if (m_setupData.wLength > Endpoint0Controller::k_maxPacketSize) {
    m_state = State::DataOut;
  } else {
    m_state = State::LastDataOut;
  }

  m_endpoint0Controller->setOutNAK(false);
}

int ControlTransferController::getDescriptor() {
  int descriptorIndex = SetupDataHelper::descriptorIndexFromWValue(m_setupData.wValue);

  switch (SetupDataHelper::descriptorTypeFromWValue(m_setupData.wValue)) {
    case USB_DT_BOS:
      m_transferBuffer = m_transferBufferInit;
      m_transferBufferLength = m_webUSBDelegate.buildBOSDescriptor(m_transferBuffer, m_transferBufferLength);
      return (int) RequestReturnCodes::Success;
    case USB_DT_DEVICE:
      m_transferBuffer = (uint8_t *)(&sDeviceDescriptor);
      m_transferBufferLength = MIN(m_transferBufferLength, sDeviceDescriptor.bLength);
      return (int) RequestReturnCodes::Success;
    case USB_DT_CONFIGURATION:
      m_transferBuffer = m_transferBufferInit;
      m_transferBufferLength = buildConfigDescriptor(descriptorIndex);
      return (int) RequestReturnCodes::Success;
    case USB_DT_STRING:
      struct StringDescriptor * stringDescriptor = (struct StringDescriptor *)m_transferBufferInit;
      if (descriptorIndex == 0) {
        /* Send language ID descriptor. */
        stringDescriptor->wData[0] = USB_LANGID_ENGLISH_US;
        stringDescriptor->bLength = sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType) +
          sizeof(stringDescriptor->wData[0]);
        m_transferBufferLength = MIN(m_transferBufferLength, stringDescriptor->bLength);
      } else if (descriptorIndex == 0xEE) {
        return m_windowsOSStringDelegate.buildDataToSend(
          m_setupData,
          &m_transferBuffer,
          &m_transferBufferLength,
          m_transferBufferInit);
      } else {
        int arrayIndex = descriptorIndex - 1;
        // Check that string index is in range.
        if (arrayIndex >= USBData::k_numberOfStrings) {
          return (int) RequestReturnCodes::Failure;
        }

        /* Strings with Language ID different from USB_LANGID_ENGLISH_US are not
         * supported */
        if (m_setupData.wIndex != USB_LANGID_ENGLISH_US) {
          return (int) RequestReturnCodes::Failure;
        }

        /* This string is returned as UTF16, hence the multiplication */
        stringDescriptor->bLength = strlen(USBData::k_strings[arrayIndex]) * 2 +
          sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType);

        m_transferBufferLength = MIN(m_transferBufferLength, stringDescriptor->bLength);

        for (int i = 0; i < (m_transferBufferLength / 2) - 1; i++) {
          stringDescriptor->wData[i] = USBData::k_strings[arrayIndex][i];
        }
      }

      stringDescriptor->bDescriptorType = USB_DT_STRING;
      m_transferBuffer = (uint8_t *)stringDescriptor;
      return (int) RequestReturnCodes::Success;
  }
  return (int) RequestReturnCodes::Failure;
}

uint16_t ControlTransferController::buildConfigDescriptor(uint8_t index) {
  uint8_t * bufferStart = m_transferBuffer;
  uint16_t count = MIN(m_transferBufferLength, sConfigDescriptor.bLength);

  memcpy(m_transferBuffer, &sConfigDescriptor, count);
  m_transferBuffer += count;
  m_transferBufferLength -= count;
  uint16_t total = count;
  uint16_t totalLength = sConfigDescriptor.bLength;

  /* For now, we have one interface only */
  assert(sConfigDescriptor.bNumInterfaces == 1);

  /* The interface has no Interface Association Descriptor and one setting only */
  /* Copy interface descriptor. */
  count = MIN(m_transferBufferLength, sInterfaceDescriptor.bLength);
  memcpy(m_transferBuffer, &sInterfaceDescriptor, count);
  total += count;
  totalLength += sInterfaceDescriptor.bLength;

  /* We have no additional endpoints for this interface */

  /* Fill in wTotalLength. */
  *(uint16_t *)(bufferStart + 2) = totalLength;
  m_transferBuffer = bufferStart;

  return total;
}

int ControlTransferController::setConfiguration() {
  // We support one configuration only
  if (m_setupData.wValue != 0 && m_setupData.wValue != USB_DFU_CONFIGURATION_VALUE) {
    return (int) RequestReturnCodes::Failure;
  }
  // There is one configuration only, we no need to set it again.

  // Reset the endpoint
  m_endpoint0Controller->reset();
  return (int) RequestReturnCodes::Success;
}

int ControlTransferController::getStatus() {
  if (m_transferBufferLength > 2) {
    m_transferBufferLength = 2;
  }
  m_transferBuffer[0] = 0;
  m_transferBuffer[1] = 0;
  return (int) RequestReturnCodes::Success;
}

int ControlTransferController::controlRequestDispatch() {
  if (SetupDataHelper::bmRequestTypeType(m_setupData.bmRequestType) == RequestTypeType::Vendor) {
    return controlCustomSetup();
  }
  return controlStandardRequest();
}

int ControlTransferController::controlStandardRequest() {
  switch (m_setupData.bRequest) {
    case USB_REQ_GET_STATUS:
      return getStatus();
      break;
    case USB_REQ_SET_ADDRESS:
      if ((m_setupData.bmRequestType != 0) || (m_setupData.wValue >= 128)) {
        return (int) RequestReturnCodes::Failure;
      }
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      m_usbDevice->setAddress(m_setupData.wValue);
      return (int) RequestReturnCodes::Success;
      break;
    case USB_REQ_GET_DESCRIPTOR:
      return getDescriptor();
      break;
    case USB_REQ_SET_CONFIGURATION:
      return setConfiguration();
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
    case USB_REQ_SET_DESCRIPTOR:
    case USB_REQ_GET_CONFIGURATION:
      // Not needed for enumeration?
      break;
    default:
      break;
  }
  return 0;
}

int ControlTransferController::controlCustomSetup() {
  switch (m_setupData.bRequest) {
    case USB_OS_STRING_BMS_VENDOR_CODE:
      return m_windowsOSStringDelegate.buildDataToSend(
          m_setupData,
          &m_transferBuffer,
          &m_transferBufferLength,
          m_transferBufferInit);
      break;
    case WEB_USB_BVENDOR_CODE:
      return m_webUSBDelegate.buildDataToSend(
          m_setupData,
          &m_transferBuffer,
          &m_transferBufferLength,
          m_transferBufferInit);
      break;
    default:
      break;
  }
  return (int) RequestReturnCodes::Failure;
}

}
}
}
