#ifndef ION_SIMULATOR_WINDOW_H
#define ION_SIMULATOR_WINDOW_H

namespace Ion {
namespace Simulator {
namespace Window {

void init(bool screen_only, bool fullscreen, bool unresizable);
void quit();

bool isHeadless();

void setNeedsRefresh();
void refresh();
void relayout();

void didRefresh();

}
}
}

#endif
