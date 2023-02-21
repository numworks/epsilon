#include "usb_connected_controller.h"

#include <apps/i18n.h>
#include <escher/palette.h>

namespace USB {

constexpr I18n::Message sUSBConnectedMessages[] = {
    I18n::Message::USBConnected,      I18n::Message::ConnectedMessage1,
    I18n::Message::ConnectedMessage2, I18n::Message::TransferDataUrl,
    I18n::Message::BlankMessage,      I18n::Message::ConnectedMessage3,
    I18n::Message::ConnectedMessage4, I18n::Message::ConnectedMessage5};

constexpr KDColor sUSBConnectedColors[] = {
    KDColorBlack, KDColorBlack, KDColorBlack, Escher::Palette::YellowDark,
    KDColorWhite, KDColorBlack, KDColorBlack, KDColorBlack};

USBConnectedController::USBConnectedController()
    : ViewController(nullptr),
      m_messageView(sUSBConnectedMessages, sUSBConnectedColors, 8) {}

}  // namespace USB
