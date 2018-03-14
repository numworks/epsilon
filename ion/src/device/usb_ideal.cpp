usb/controller.h
usb/std/device.h
usb/std/configuration.h
usb/std/endpoint.h
usb/calculator.h
usb/dfu.h


/* libopencm3 */

On crée un device
On enregistre de control_callback
.. Et voilà !
 -> En fait, les control_callbacks, on va les faire avec la virtualité
 au nivua du device


Poll -> ENDPOINT -> Configuration.. Fini!

Controller::poll -> Device -> Endpoint -> Configuration

Controller::poll
 -> Interruption (quel type, SETUP/OUT et quel EP)
 -> 



control_transfer
bulk_transfer

class DFU : public Configuration {
  virtual void didReceivePacket();
};

// Lifecycle des messages :
//
// 1/ Dans poll, je reçois RXFLVL : des données sont arrivées
// 2/ Je regarde à, quel endpoint elles correspondent
// 3/ Si c'est un setup, truchmuche
//    Si c'est un OUT, je le refile au Endpoint


usb_controller(device);
usb_controller->poll();

class Endpoint {
  Endpoint(number, type, etc...);
};

class Configuration {
  Configuration(Endpoints);
  virtual void didReceiveData() = 0;
  void sendData();
}


class Device {
public:
  Device(uint16_t vid,
      uint16_t pid,
      const char * serialNumber,
      const char * vendor,
      tableau_de_device_capabilities,
      les_configurations /* e.g. DFU, HID, MSC, etc... */
      ) :
    m_vid(vid),
    m_tableau_de_dev_cap(tableau_de_device_capabilities),
    m_tableau_de_configuration(les_configurations)
{

  // Sur chaque Endpoint, méthode "

}

};

class Calculator : public USBDevice {
  Calculator();
};


class DFUConfiguration : public USBConfiguration {
  void didReceiveData() override {
    // Ecrire à, l'adresse passée en paramètre les données passées en paramètre
    // Retourner "OK" en USB :
    sendData("OK");
  }
  // Surement des trucs à faire ici
}

class USB::Device::BOSDescriptor {
  BOSDescriptor(capabilities);
};

class WebUSBBOSDescriptor : BOSDescriptor {
  WebUSBDescriptor(const char * URL);
};

class WebUSBDeviceCapability : BOSCapability {
};

Calculator::Calculator() :
  USBDevice(
      0x123 /* VID*/,
      0x456 /* PID */
      "serialNumber",
      "VendorString",
      "ProductString",
      { WebUSBDeviceCapability("https://dedeed") } // BOS descriptor
      ) {

    // A un moment donné, on va recevoir des requêtes "getStringDescriptor"
    // -> Il faudra aller voir soit dans les const char * du device
    //    soit dans les const char * des capabilities

    addConfiguration(new DFUConfiguration());

}
