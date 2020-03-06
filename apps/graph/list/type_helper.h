#ifndef GRAPH_LIST_TYPE_HELPER_H
#define GRAPH_LIST_TYPE_HELPER_H

#include <escher/i18n.h>
#include <poincare/layout.h>

namespace Graph {

namespace PlotTypeHelper {

constexpr static int NumberOfTypes = 3;
I18n::Message Message(int index);
Poincare::Layout Layout(int index);

}

}

#endif
