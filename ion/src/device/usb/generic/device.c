namespace Ion {
namespace USB {

/* We only handle control transfers (on EP0 then) */

class Device {
public:
  Device(uint16_t vid, uint16_t pid, const char * serialNumber,
      const char * vendor, tableau_de_device_capabilities, les_configurations /* e.g. DFU, HID, MSC, etc... */
      ) :
    m_vid(vid),
    m_tableau_de_dev_cap(tableau_de_device_capabilities),
/*    m_tableau_de_configuration(les_configurations)*/
  {
  }

  void init();
  void shutdown();
  void poll() {
    if (setup) {
      EP0->readSetupData(); // Retrieve SETUP data from RxFIFO
    } else if (in) {
      EP0->processINpacket();
    } else if (out) {
      EP0->processOUTpacket();
    }
  }

  virtual bool controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

private:
  Endpoint0 m_ep0;


  /*usbd_register_control_callback(uint8_t type,
    uint8_t type_mask,
    usbd_control_callback callback);
    */


};

}
}

