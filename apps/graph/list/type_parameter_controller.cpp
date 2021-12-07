#include "type_parameter_controller.h"
#include <poincare/layout_helper.h>
#include <poincare/expression.h>
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>
#include "../../shared/poincare_helpers.h"

using namespace Escher;

namespace Graph {

TypeParameterController::TypeParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_record()
{
}

void TypeParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

const char * TypeParameterController::title() {
  return I18n::translate(I18n::Message::Details);
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  assert(!m_record.isNull());
  selectCellAtLocation(0, 0);
  resetMemoization();
  m_selectableTableView.reloadData();
}

KDCoordinate TypeParameterController::nonMemoizedRowHeight(int j) {
  MessageTableCellWithMessageWithBuffer tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

void TypeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < k_numberOfDataPoints);
  MessageTableCellWithMessageWithBuffer * myCell = static_cast<MessageTableCellWithMessageWithBuffer *>(cell);
  if (index == 0) {
    myCell->setMessage(I18n::Message::CurveType);
    myCell->setSubLabelMessage(I18n::Message::Default);
    myCell->setAccessoryText(I18n::translate(function()->plotTypeMessage()));
  } else {
    myCell->setMessage(detailsTitle(index - 1));
    double value = detailsValue(index - 1);
    if (std::isnan(value)) {
      // There are no value to display, use the accessory for description
      myCell->setAccessoryText(I18n::translate(detailsDescription(index - 1)));
      myCell->setSubLabelMessage(I18n::Message::Default);
    } else {
      constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
      constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
      char buffer[bufferSize];
      Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
      myCell->setAccessoryText(buffer);
      myCell->setSubLabelMessage(detailsDescription(index - 1));
    }
  }
}

MessageTableCellWithMessageWithBuffer * TypeParameterController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return &m_cells[index];
}

void TypeParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  if (!m_record.isNull()) {
    Shared::ExpiringPointer<Shared::ContinuousFunction> f = function();
    Poincare::Context * context = App::app()->localContext();
    if (f->plotType() == Shared::ContinuousFunction::PlotType::Line) {
      double slope, intercept;
      f->getLineParameters(&slope, &intercept, context);
      setLineDetailsValues(slope, intercept);
    } else if (f->isConic()) {
      setConicDetailsValues(f->getConicParameters(context));
    }
  }
}

int TypeParameterController::detailsNumberOfSections() const {
  switch (function()->plotType()) {
    case Shared::ContinuousFunction::PlotType::Line:
      return k_lineDetailsSections;
    case Shared::ContinuousFunction::PlotType::Circle:
      return k_circleDetailsSections;
    case Shared::ContinuousFunction::PlotType::Ellipse:
      return k_ellipseDetailsSections;
    case Shared::ContinuousFunction::PlotType::CartesianParabola:
    case Shared::ContinuousFunction::PlotType::Parabola:
      return k_parabolaDetailsSections;
    case Shared::ContinuousFunction::PlotType::CartesianHyperbola:
    case Shared::ContinuousFunction::PlotType::Hyperbola:
      return k_hyperbolaDetailsSections;
    default:
      return 0;
  }
}

Shared::ExpiringPointer<Shared::ContinuousFunction> TypeParameterController::function() const {
  assert(!m_record.isNull());
  App * myApp = App::app();
  return myApp->functionStore()->modelForRecord(m_record);
}

I18n::Message TypeParameterController::detailsTitle(int i) const {
  assert(i < detailsNumberOfSections());
  switch (function()->plotType()) {
    case Shared::ContinuousFunction::PlotType::Line: {
      constexpr I18n::Message k_titles[k_lineDetailsSections] = {
          I18n::Message::LineEquationTitle,
          I18n::Message::LineSlopeTitle,
          I18n::Message::LineYInterceptTitle,
      };
      return k_titles[i];
    }
    case Shared::ContinuousFunction::PlotType::Circle: {
      constexpr I18n::Message k_titles[k_circleDetailsSections] = {
          I18n::Message::CircleRadiusTitle,
          I18n::Message::CenterAbscissaTitle,
          I18n::Message::CenterOrdinateTitle,
      };
      return k_titles[i];
    }
    case Shared::ContinuousFunction::PlotType::Ellipse: {
      constexpr I18n::Message k_titles[k_ellipseDetailsSections] = {
          I18n::Message::EllipseSemiMajorAxisTitle,
          I18n::Message::EllipseSemiMinorAxisTitle,
          I18n::Message::LinearEccentricityTitle,
          I18n::Message::EccentricityTitle,
          I18n::Message::CenterAbscissaTitle,
          I18n::Message::CenterOrdinateTitle,
      };
      return k_titles[i];
    }
    case Shared::ContinuousFunction::PlotType::CartesianParabola:
    case Shared::ContinuousFunction::PlotType::Parabola: {
      constexpr I18n::Message k_titles[k_parabolaDetailsSections] = {
          I18n::Message::ParabolaParameterTitle,
          I18n::Message::ParabolaVertexAbscissaTitle,
          I18n::Message::ParabolaVertexOrdinateTitle,
      };
      return k_titles[i];
    }
    default: {
      assert(function()->plotType() == Shared::ContinuousFunction::PlotType::CartesianHyperbola
             || function()->plotType() == Shared::ContinuousFunction::PlotType::Hyperbola);
      constexpr I18n::Message k_titles[k_hyperbolaDetailsSections] = {
          I18n::Message::HyperbolaSemiMajorAxisTitle,
          I18n::Message::HyperbolaSemiMinorAxisTitle,
          I18n::Message::LinearEccentricityTitle,
          I18n::Message::EccentricityTitle,
          I18n::Message::CenterAbscissaTitle,
          I18n::Message::CenterOrdinateTitle,
      };
      return k_titles[i];
    }
  }
}

I18n::Message TypeParameterController::detailsDescription(int i) const {
  assert(i < detailsNumberOfSections());
  switch (function()->plotType()) {
    case Shared::ContinuousFunction::PlotType::Line: {
      constexpr I18n::Message k_descriptions[k_lineDetailsSections] = {
          I18n::Message::LineEquationDescription,
          I18n::Message::LineSlopeDescription,
          I18n::Message::LineYInterceptDescription,
      };
      return k_descriptions[i];
    }
    case Shared::ContinuousFunction::PlotType::Circle: {
      constexpr I18n::Message k_descriptions[k_circleDetailsSections] = {
          I18n::Message::CircleRadiusDescription,
          I18n::Message::CenterAbscissaDescription,
          I18n::Message::CenterOrdinateDescription,
      };
      return k_descriptions[i];
    }
    case Shared::ContinuousFunction::PlotType::Ellipse: {
      constexpr I18n::Message k_descriptions[k_ellipseDetailsSections] = {
          I18n::Message::EllipseSemiMajorAxisDescription,
          I18n::Message::EllipseSemiMinorAxisDescription,
          I18n::Message::LinearEccentricityDescription,
          I18n::Message::EccentricityDescription,
          I18n::Message::CenterAbscissaDescription,
          I18n::Message::CenterOrdinateDescription,
      };
      return k_descriptions[i];
    }
    case Shared::ContinuousFunction::PlotType::CartesianParabola:
    case Shared::ContinuousFunction::PlotType::Parabola: {
      constexpr I18n::Message k_descriptions[k_parabolaDetailsSections] = {
          I18n::Message::ParabolaParameterDescription,
          I18n::Message::ParabolaVertexAbscissaDescription,
          I18n::Message::ParabolaVertexOrdinateDescription,
      };
      return k_descriptions[i];
    }
    default: {
      assert(function()->plotType() == Shared::ContinuousFunction::PlotType::CartesianHyperbola
             || function()->plotType() == Shared::ContinuousFunction::PlotType::Hyperbola);
      constexpr I18n::Message k_descriptions[k_hyperbolaDetailsSections] = {
          I18n::Message::HyperbolaSemiMajorAxisDescription,
          I18n::Message::HyperbolaSemiMinorAxisDescription,
          I18n::Message::LinearEccentricityDescription,
          I18n::Message::EccentricityDescription,
          I18n::Message::CenterAbscissaDescription,
          I18n::Message::CenterOrdinateDescription,
      };
      return k_descriptions[i];
    }
  }
}

void TypeParameterController::setLineDetailsValues(double slope, double intercept) {
  assert(function()->plotType() == Shared::ContinuousFunction::PlotType::Line);
  m_detailValues[0] = NAN;
  m_detailValues[1] = slope;
  m_detailValues[2] = intercept;
}

void TypeParameterController::setConicDetailsValues(Poincare::Conic conic) {
  Shared::ContinuousFunction::PlotType type = function()->plotType();
  double cx, cy;
  if (type == Shared::ContinuousFunction::PlotType::CartesianParabola
      || type == Shared::ContinuousFunction::PlotType::Parabola) {
    conic.getSummit(&cx, &cy);
  } else {
    conic.getCenter(&cx, &cy);
  }
  if (type == Shared::ContinuousFunction::PlotType::Circle) {
    m_detailValues[0] = conic.getRadius();
    m_detailValues[1] = cx;
    m_detailValues[2] = cy;
    return;
  }
  if (type == Shared::ContinuousFunction::PlotType::Ellipse) {
    m_detailValues[0] = conic.getSemiMajorAxis();
    m_detailValues[1] = conic.getSemiMinorAxis();
    m_detailValues[2] = conic.getLinearEccentricity();
    m_detailValues[3] = conic.getEccentricity();
    m_detailValues[4] = cx;
    m_detailValues[5] = cy;
    return;
  }
  if (type == Shared::ContinuousFunction::PlotType::CartesianParabola
      || type == Shared::ContinuousFunction::PlotType::Parabola) {
    m_detailValues[0] = conic.getParameter();
    m_detailValues[1] = cx;
    m_detailValues[2] = cy;
    return;
  }
  assert(type == Shared::ContinuousFunction::PlotType::CartesianHyperbola
         || type == Shared::ContinuousFunction::PlotType::Hyperbola);
  m_detailValues[0] = conic.getSemiMajorAxis();
  m_detailValues[1] = conic.getSemiMinorAxis();
  m_detailValues[2] = conic.getLinearEccentricity();
  m_detailValues[3] = conic.getEccentricity();
  m_detailValues[4] = cx;
  m_detailValues[5] = cy;
  return;
}

StackViewController * TypeParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

}
