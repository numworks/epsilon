#ifndef SEQUENCE_GRAPH_CONTROLLER_H
#define SEQUENCE_GRAPH_CONTROLLER_H

#include "graph_view.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "curve_view_range.h"
#include "../../shared/function_graph_controller.h"
#include "../sequence_store.h"

namespace Sequence {

class GraphController : public Shared::FunctionGraphController {
public:
  GraphController(Responder * parentResponder, SequenceStore * sequenceStore, HeaderViewController * header);
  const char * emptyMessage() override;
private:
  BannerView * bannerView() override;
  bool moveCursorHorizontally(int direction) override;
  void initCursorParameters() override;
  CurveViewRange * interactiveCurveViewRange() override;
  SequenceStore * functionStore() const override;
  GraphView * functionGraphView() override;
  CurveParameterController * curveParameterController() override;
  BannerView m_bannerView;
  GraphView m_view;
  CurveViewRange m_graphRange;
  CurveParameterController m_curveParameterController;
  SequenceStore * m_sequenceStore;
};


}

#endif
