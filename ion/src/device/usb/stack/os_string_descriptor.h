class OSStringDescriptor : public Descriptor {
public:
  StringDescriptor(const char * string);
  void copy(void * target) override {
    memcpy(this, target, sizeof(Descriptor));
    // Ensuite, on veut pousser le compat ID et les extended properties
    ;
  }
private:
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t qwSignature[14];
  uint8_t bMS_VendorCode;
  uint8_t bPad;
};

