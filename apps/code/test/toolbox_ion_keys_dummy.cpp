#include "../toolbox_ion_keys.h"

namespace Code {
  toolboxIonKeys::toolboxIonKeys() :
  ViewController(nullptr),
  m_view()
  {
  }

  bool toolboxIonKeys::handleEvent(Ion::Events::Event e) {
    return false;
  }

  toolboxIonKeys::toolboxIonView::toolboxIonView():
    View()
    {
    }

  void toolboxIonKeys::toolboxIonView::drawRect(KDContext * ctx, KDRect rect) const {
    return;
  }

  View * toolboxIonKeys::view(){
    return &m_view;
  }

}
