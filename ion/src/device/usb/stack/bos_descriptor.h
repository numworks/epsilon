class BOSDescriptor : public Descriptor {
public:
      BOSDescriptor(const CapabilityDescriptor * capabilityDescriptors, int number) :
      Descriptor(...)
      m_wTotalLength(somme des size des capDesc),
      bNumDeviceCaps(number)
    {
    }
  uint16_t copy(void * target) override {
    super::copy();
    for (capability in m_deviceCapabilities) {
      capability->copy(target+machinchose,...);
    }
private:
    uint16_t wTotalLength; // Length of this descriptor and all of its sub descriptors
    uint8_t bNumDeviceCaps; // The number of separate device capability descriptors in the BOS

  DeviceCapabilityDescriptor * m_deviceCapabilities;
} __attribute__((packed));

static_assert(sizeof(DeviceDescriptor) == 12);
