#include "toolbox_ion_keys.h"
#include <apps/apps_container.h>
#include <assert.h>
#include <ion.h>
extern "C" {
#include <py/obj.h>
#include <py/objfun.h>
}
extern "C" const mp_rom_map_elem_t modion_module_globals_table[52];

namespace Code {
  toolboxIonKeys::toolboxIonKeys() :
  ViewController(nullptr),
  m_view()
  {
  }

  bool toolboxIonKeys::handleEvent(Ion::Events::Event e) {
    Ion::Keyboard::State state = Ion::Keyboard::scan();
    for(uint16_t i = 0; i < sizeof(modion_module_globals_table)/sizeof(_mp_rom_map_elem_t); i++){
      _mp_rom_map_elem_t element = modion_module_globals_table[i];
      if(mp_obj_is_small_int(element.value)){
        int key = mp_obj_get_int(element.value);
        if(state.keyDown(static_cast<Ion::Keyboard::Key>(key))){
          m_sender->handleEventWithText(qstr_str(MP_OBJ_QSTR_VALUE(element.key)), true);
        }
      }
    }
    Container::activeApp()->dismissModalViewController();
    AppsContainer::sharedAppsContainer()->redrawWindow();
    return true;
  }

  toolboxIonKeys::toolboxIonView::toolboxIonView():
    View()
    {
    }

  void toolboxIonKeys::toolboxIonView::drawRect(KDContext * ctx, KDRect rect) const {
    ctx->fillRect(rect, Palette::WallScreen);
    ctx->strokeRect(rect, Palette::ListCellBorder);
    ctx->drawString(I18n::translate(I18n::Message::PressAKey),KDPoint(rect.left()+80, rect.top()+20),KDFont::LargeFont,Palette::PrimaryText,Palette::WallScreen);
  
  }

  View * toolboxIonKeys::view(){
    return &m_view;
  }

}
