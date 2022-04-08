#include <ion.h>
#include <kandinsky.h>
#include "image.h"

namespace Flasher {
    namespace Display {

        constexpr static int sNumberOfMessages = 5;

        constexpr static const char * sMessages[sNumberOfMessages] = {
            "RECOVERY MODE",
            "Your calculator is waiting",
            "for Upsilon to be installed.",
            "Follow the instructions",
            "on your computer to continue.",
        };

        void init() {
            KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
            Ion::Display::pushRectUniform(screen, KDColor::RGB24(0x2B2B2B));
            KDContext * ctx = KDIonContext::sharedContext();
            ctx->setOrigin(KDPointZero);
            ctx->setClippingRect(screen);
            KDCoordinate margin = 30;
            KDCoordinate currentHeight = margin;

            /* Title */
            const char * title = sMessages[0];
            KDSize titleSize = KDFont::LargeFont->stringSize(title);
            ctx->drawString(title, KDPoint((Ion::Display::Width-titleSize.width())/2, currentHeight),
                            KDFont::LargeFont, KDColorWhite, KDColor::RGB24(0x2B2B2B));
            currentHeight = (uint16_t)((Ion::Display::Height*2)/3);

            /* Logo */
            for (int i = 0; i < IMAGE_WIDTH; ++i) {
                for (int j = 0; j < IMAGE_HEIGHT; ++j) {
                    ctx->setPixel(KDPoint(i+(uint16_t)((Ion::Display::Width-IMAGE_WIDTH)/2),
                                          j+(titleSize.height()+margin+15)),
                                  KDColor::RGB16(image[i+(j*IMAGE_WIDTH)]));
                }
            }

            /* Messages */
            const char * message;
            for (int i = 1; i < sNumberOfMessages; ++i) {
                message = sMessages[i];
                KDSize messageSize = KDFont::SmallFont->stringSize(message);
                ctx->drawString(message, KDPoint((Ion::Display::Width-messageSize.width())/2, currentHeight),
                                KDFont::SmallFont, KDColorWhite, KDColor::RGB24(0x2B2B2B));
                currentHeight += messageSize.height();
            }
        }

    }
}
