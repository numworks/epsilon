namespace Ion {
namespace USB {

/* We only handle control transfers, on EP0 then. */

class Device {
public:
  Device(uint16_t vid, uint16_t pid, const char * serialNumber,
      const char * vendor, tableau_de_device_capabilities, les_configurations /* e.g. DFU, HID, MSC, etc... */
      ) :
    m_vid(vid),
    m_pid(pid),
    m_serialNumber(serialNumber),
    m_vendor(vendor),
    m_tableau_de_dev_cap(tableau_de_device_capabilities),
/*    m_tableau_de_configuration(les_configurations)*/
  {
  }

  void init();
  void shutdown();
  void poll();

  virtual bool controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

private:
  uint16_t m_vid;
  uint16_t m_pid;
  const char * m_serialNumber;
  const char * m_vendor;
  Endpoint0 m_ep0;
};

}
}
