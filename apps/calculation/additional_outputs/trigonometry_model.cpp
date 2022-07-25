#include "trigonometry_model.h"
#include "trigonometry_list_controller.h"

namespace Calculation {

TrigonometryModel::TrigonometryModel() :
  Shared::CurveViewRange(),
  m_angle(NAN)
{
}

float TrigonometryModel::yHalfRange() const {
  return TrigonometryListController::k_illustrationHeight*k_xHalfRange/(Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin);
}

}
