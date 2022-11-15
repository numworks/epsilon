#include "graph_controller.h"
#include <cmath>
#include <limits.h>
#include "../app.h"
#include <float.h>
#include <cmath>
#include <algorithm>
#include <apps/i18n.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, CurveViewRange * graphRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, graphRange, &m_view, cursor, indexFunctionSelectedByCursor),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(sequenceStore, graphRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(graphRange),
  m_curveParameterController(inputEventHandlerDelegate, this, &m_cobwebController, graphRange, m_cursor),
  m_sequenceSelectionController(this),
  m_termSumController(this, inputEventHandlerDelegate, &m_view, graphRange, m_cursor),
  m_cobwebController(this, inputEventHandlerDelegate, this, &m_view, graphRange, m_cursor, &m_bannerView, &m_cursorView, sequenceStore),
  m_sequenceStore(sequenceStore)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

void GraphController::viewWillAppear() {
  m_view.setCursorView(&m_cursorView);
  m_cursorView.resetMemoization();
  m_smallestRank = m_sequenceStore->smallestInitialRank();
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor(), true);
}

float GraphController::interestingXMin() const {
  int nmin = INT_MAX;
  int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < nbOfActiveModels; i++) {
    Shared::Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    nmin = std::min(nmin, s->initialRank());
  }
  assert(nmin < INT_MAX);
  return nmin;
}

bool GraphController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  double floatBody;
  if (myApp->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  floatBody = std::fmax(0, std::round(floatBody));
  moveToRank(floatBody);
  return true;
}

void GraphController::moveToRank(int n) {
  double y = xyValues(selectedCurveRelativePosition(), n, textFieldDelegateApp()->localContext()).x2();
  m_cursor->moveTo(n, n, y);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  reloadBannerView();
  m_view.reload();
}

Range2D GraphController::optimalRange(bool computeX, bool computeY, Range2D originalRange) const {
  Zoom::Function2DWithContext<float> evaluator = [](float x, const void * model, Context * context) {
    const Shared::Sequence * s = static_cast<const Shared::Sequence *>(model);
    return s->evaluateXYAtParameter(x, context);
  };

  Range2D result;
  if (computeX) {
    float xMin = interestingXMin();
    *result.x() = Range1D(xMin, xMin + k_defaultXHalfRange);
  } else {
    *result.x() = *originalRange.x();
  }
  if (computeY) {
    Zoom zoom(result.xMin(), result.xMax(), InteractiveCurveViewRange::NormalYXRatio(), textFieldDelegateApp()->localContext(), k_maxFloat);
    int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
    for (int i = 0; i < nbOfActiveModels; i++) {
      Shared::Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
      zoom.fitFullFunction(evaluator, s);
    }
    *result.y() = *zoom.range(true, false).y();
  }
  return Zoom::Sanitize(result, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
}

Layout GraphController::SequenceSelectionController::nameLayoutAtIndex(int j) const {
  GraphController * graphController = static_cast<GraphController *>(m_graphController);
  SequenceStore * store = graphController->functionStore();
  ExpiringPointer<Shared::Sequence> sequence = store->modelForRecord(store->activeRecordAtIndex(j));
  return sequence->definitionName().clone();
}

void GraphController::selectFunctionWithCursor(int functionIndex, bool willBeVisible) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex, willBeVisible);
  ExpiringPointer<Shared::Sequence> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex));
  m_cursorView.setColor(f->color());
}

bool GraphController::openMenuForCurveAtIndex(int index) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(index);
  m_termSumController.setRecord(record);
  return FunctionGraphController::openMenuForCurveAtIndex(index);
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  double xCursorPosition = std::round(m_cursor->x());
  if (direction < 0 && xCursorPosition <= m_smallestRank) {
    return false;
  }
  // The cursor moves by step that is larger than 1 and than a pixel's width.
  const int step = std::ceil(m_view.pixelWidth()) * scrollSpeed;
  double x = direction > 0 ? xCursorPosition + step:
    xCursorPosition -  step;
  if (x < 0.0) {
    return false;
  }
  Shared::Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
  double y = s->evaluateXYAtParameter(x, textFieldDelegateApp()->localContext()).x2();
  m_cursor->moveTo(x, x, y);
  return true;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  return std::fmax(0.0, std::round(Shared::FunctionGraphController::defaultCursorT(record)));
}

}
