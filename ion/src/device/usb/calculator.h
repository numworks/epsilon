namespace Ion {
namespace USB {

class Calculator : public Device {
public:
  Calculator();
  void controlTransfer() override;
}

Calculator::controlTransfer() {
  if (Device::controlTransfer()){
    return;
  }
  // Do DFU stuff
}

}
