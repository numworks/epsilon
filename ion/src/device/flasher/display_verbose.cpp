#include <ion.h>
#include <kandinsky.h>

namespace Flasher {
namespace Display {

constexpr static int sNumberOfMessages = 5;
constexpr static int sNumberOfLanguages = 2;

constexpr static const char * sMessages[sNumberOfLanguages][sNumberOfMessages] = {
  {"RECOVERY MODE",
    "Your calculator is waiting",
    "for a new software.",
    "Follow the instructions",
    "on your computer to continue."},
  {"MODE RECUPERATION",
    "Votre calculatrice attend",
    "l'installation d'un nouveau logiciel.",
    "Suivez les instructions sur",
    "votre ordinateur pour continuer."}
};

void init() {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(screen, KDColorWhite);
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  KDCoordinate margin = 20;
  KDCoordinate currentHeight = 0;
  for (int i = 0; i < sNumberOfLanguages; i++) {
    currentHeight += margin;
    const char * title = sMessages[i][0];
    KDSize titleSize = KDFont::LargeFont->stringSize(title);
    ctx->drawString(title, KDPoint((Ion::Display::Width-titleSize.width())/2, currentHeight), KDFont::LargeFont);
    currentHeight += 2*titleSize.height();
    for (int j = 1; j < sNumberOfMessages; j++) {
      const char * message = sMessages[i][j];
      KDSize messageSize = KDFont::SmallFont->stringSize(message);
      ctx->drawString(message, KDPoint((Ion::Display::Width-messageSize.width())/2, currentHeight), KDFont::SmallFont);
      currentHeight += messageSize.height();
    }
  }
}

}
}
