#ifndef SEQUENCE_GRAPH_VIEW_H
#define SEQUENCE_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"
#include "../../shared/sequence_store.h"

namespace Sequence {

class GraphView : public Shared::FunctionGraphView {
public:
  GraphView(Shared::SequenceStore * sequenceStore, Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  Shared::SequenceStore * m_sequenceStore;
};

}

#endif
