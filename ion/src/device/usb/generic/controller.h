Poll->
  Interruption (type SETUP/OUT, et le numéro de EP assert(0))
  Ma question : moulinette pour passer de "poll" à "control_transfer_callback"

  EP0 ->

  void poll() {
    if (setup) {
      EP0->readSetupData(); // Retrieve SETUP data from RxFIFO
    } else if (in) {
      EP0->processINpacket();
    } else if (out) {
      EP0->processOUTpacket();
    }


  }

class SetupPacket {
};


class EP0 {
  readSetupData() {
    SetupRequest req = Pop RxFIFO
    if (req.bRequest means IN transfer) {
      device.controlCallback(req, m_buffer, &m_bufferLength);
      TxFIFO.push(m_buffer.first_packet);
    }
  }

  processINpacket() {
    if (en_cours_de_route) {
      TxFIFO.push(m_buffer.nextPacket);
    } else {
      // On a fini de faire plein de OUT
      // On a fini de recevoir
      TxFIFO.push(vide); // On confirm
      device.controlCallback(req, m_buffer, &m_bufferLength);
    }
  }

  processOUTpacket() {
    m_buffer.append(RxFIFO.pop);
  }


private:
  char m_buffer[2048];
  int m_bufferLength;
}
