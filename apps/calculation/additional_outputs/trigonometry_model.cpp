#include "trigonometry_model.h"
#include "trigonometry_list_controller.h"

namespace Calculation {

TrigonometryModel::TrigonometryModel() :
  Shared::CurveViewRange(),
  m_angle(NAN)
{
}

float TrigonometryModel::yHalfRange() const {
  return yRange() / 2.f;
}

float TrigonometryModel::yRange() const {
  return (TrigonometryListController::k_illustrationHeight - 2) * xRange() / 263.f;
}

}
