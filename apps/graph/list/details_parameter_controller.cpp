#include "details_parameter_controller.h"
#include <apps/exam_mode_configuration.h>
#include <poincare/layout_helper.h>
#include <poincare/expression.h>
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>
#include "../../shared/poincare_helpers.h"

using namespace Escher;

namespace Graph {

DetailsParameterController::DetailsParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder)
{
}

void DetailsParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool DetailsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

const char * DetailsParameterController::title() {
  return I18n::translate(I18n::Message::Details);
}

void DetailsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  assert(!m_record.isNull());
  selectCellAtLocation(0, 0);
  resetMemoization();
  m_selectableTableView.reloadData();
}

KDCoordinate DetailsParameterController::nonMemoizedRowHeight(int j) {
  MessageTableCellWithMessageWithBuffer tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

void DetailsParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < k_numberOfDataPoints);
  MessageTableCellWithMessageWithBuffer * myCell = static_cast<MessageTableCellWithMessageWithBuffer *>(cell);
  if (index == 0) {
    myCell->setMessage(I18n::Message::CurveType);
    myCell->setSubLabelMessage(I18n::Message::Default);
    myCell->setAccessoryText(I18n::translate(function()->plotTypeMessage()));
  } else {
    myCell->setMessage(detailsTitle(index - 1));
    double value = detailsValue(index - 1);
    if (index - 1 == 0 && function()->plotType() == Shared::ContinuousFunction::PlotType::Line) {
      assert(std::isnan(value));
      /* For the line's equation cell, we want the detail description (y=mx+b)
       * to be displayed as the value would : a large font accessory. */
      myCell->setAccessoryText(I18n::translate(detailsDescription(index - 1)));
      myCell->setSubLabelMessage(I18n::Message::Default);
    } else {
      constexpr int precision = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
      constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
      char buffer[bufferSize];
      Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
      myCell->setAccessoryText(buffer);
      myCell->setSubLabelMessage(detailsDescription(index - 1));
    }
  }
}

MessageTableCellWithMessageWithBuffer * DetailsParameterController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return &m_cells[index];
}

void DetailsParameterController::setRecord(Ion::Storage::Record record) {
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

int DetailsParameterController::detailsNumberOfSections() const {
  if (m_record.isNull()) {
    return 0;
  }
  switch (function()->plotType()) {
    case Shared::ContinuousFunction::PlotType::Line:
      return ExamModeConfiguration::lineDetailsAreForbidden() ? 0 : k_lineDetailsSections;
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

Shared::ExpiringPointer<Shared::ContinuousFunction> DetailsParameterController::function() const {
  assert(!m_record.isNull());
  App * myApp = App::app();
  return myApp->functionStore()->modelForRecord(m_record);
}

I18n::Message DetailsParameterController::detailsTitle(int i) const {
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
      assert(Shared::ContinuousFunction::IsPlotTypeHyperbola(function()->plotType()));
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

I18n::Message DetailsParameterController::detailsDescription(int i) const {
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
      assert(Shared::ContinuousFunction::IsPlotTypeHyperbola(function()->plotType()));
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

void DetailsParameterController::setLineDetailsValues(double slope, double intercept) {
  assert(function()->plotType() == Shared::ContinuousFunction::PlotType::Line);
  m_detailValues[0] = NAN;
  m_detailValues[1] = slope;
  m_detailValues[2] = intercept;
}

void DetailsParameterController::setConicDetailsValues(Poincare::Conic conic) {
  Shared::ContinuousFunction::PlotType type = function()->plotType();
  double cx, cy;
  if (Shared::ContinuousFunction::IsPlotTypeParabola(function()->plotType())) {
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
  if (Shared::ContinuousFunction::IsPlotTypeParabola(function()->plotType())) {
    m_detailValues[0] = conic.getParameter();
    m_detailValues[1] = cx;
    m_detailValues[2] = cy;
    return;
  }
  assert(Shared::ContinuousFunction::IsPlotTypeHyperbola(function()->plotType()));
  m_detailValues[0] = conic.getSemiMajorAxis();
  m_detailValues[1] = conic.getSemiMinorAxis();
  m_detailValues[2] = conic.getLinearEccentricity();
  m_detailValues[3] = conic.getEccentricity();
  m_detailValues[4] = cx;
  m_detailValues[5] = cy;
  return;
}

StackViewController * DetailsParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

}
