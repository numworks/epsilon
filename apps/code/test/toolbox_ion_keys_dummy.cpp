#include "../toolbox_ion_keys.h"

namespace Code {
  ToolboxIonKeys::ToolboxIonKeys() :
  ViewController(nullptr),
  m_view()
  {
  }

  bool ToolboxIonKeys::handleEvent(Ion::Events::Event e) {
    return false;
  }

  ToolboxIonKeys::toolboxIonView::toolboxIonView():
    View()
    {
    }

  void ToolboxIonKeys::toolboxIonView::drawRect(KDContext * ctx, KDRect rect) const {
    return;
  }

  View * ToolboxIonKeys::view(){
    return &m_view;
  }

}
