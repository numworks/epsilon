#include "graph/graph_app.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

void ion_app() {
#if USE_PIC_VIEW_APP
  PicViewApp picViewApp = PicViewApp();
  picViewApp.run();
#endif
  GraphApp graphApp = GraphApp();
  graphApp.run();
}
