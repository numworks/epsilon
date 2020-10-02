#ifndef ION_SIMULATOR_WINDOW_H
#define ION_SIMULATOR_WINDOW_H

namespace Ion {
namespace Simulator {
namespace Window {

void init();
void shutdown();

bool isHeadless();

void setNeedsRefresh();
void refresh();
void relayout();

void didRefresh();

}
}
}

#endif
