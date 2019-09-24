#ifndef ION_SDL_MAIN_H
#define ION_SDL_MAIN_H

namespace Ion {
namespace SDL {
namespace Main {

void init();
void quit();

void setNeedsRefresh();
void refresh();
void relayout();

}
}
}

#endif
