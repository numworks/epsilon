#include "zoom_parameter_controller.h"
#include <assert.h>
#include <math.h>

namespace Graph {

constexpr KDColor ZoomParameterController::ContentView::LegendView::k_legendBackgroundColor;

/* Zoom Parameter Controller */

ZoomParameterController::ZoomParameterController(Responder * parentResponder, AxisInterval * axisInterval, GraphView * graphView) :
  ViewController(parentResponder),
  m_contentView(ContentView(graphView)),
  m_axisInterval(axisInterval)
{
}

const char * ZoomParameterController::title() const {
  return "Zoom interactif";
}

View * ZoomParameterController::view() {
  return &m_contentView;
}

bool ZoomParameterController::handleEvent(Ion::Events::Event event) {
  float xMin = m_axisInterval->xMin();
  float xMax = m_axisInterval->xMax();
  float yMin = m_axisInterval->yMin();
  float yMax = m_axisInterval->yMax();
  m_axisInterval->setYAuto(false);
  if (event == Ion::Events::Plus) {
    m_axisInterval->setXMin((xMax+xMin)/2.0f - fabsf(xMax-xMin)/3.0f);
    m_axisInterval->setXMax((xMax+xMin)/2.0f + fabsf(xMax-xMin)/3.0f);
    m_axisInterval->setYMin((yMax+yMin)/2.0f - fabsf(yMax-yMin)/3.0f);
    m_axisInterval->setYMax((yMax+yMin)/2.0f + fabsf(yMax-yMin)/3.0f);
    m_contentView.graphView()->reload();
    return true;
  }
  if (event == Ion::Events::Minus) {
    m_axisInterval->setXMin((xMax+xMin)/2.0f - 3.0f*fabsf(xMax-xMin)/4.0f);
    m_axisInterval->setXMax((xMax+xMin)/2.0f + 3.0f*fabsf(xMax-xMin)/4.0f);
    m_axisInterval->setYMin((yMax+yMin)/2.0f - 3.0f*fabsf(yMax-yMin)/4.0f);
    m_axisInterval->setYMax((yMax+yMin)/2.0f + 3.0f*fabsf(yMax-yMin)/4.0f);
    m_contentView.graphView()->reload();
    return true;
  }
  if (event == Ion::Events::Up) {
    m_axisInterval->setYMin(yMin + m_axisInterval->yScale());
    m_axisInterval->setYMax(yMax + m_axisInterval->yScale());
    m_contentView.graphView()->reload();
    return true;
  }
  if (event == Ion::Events::Down) {
    m_axisInterval->setYMin(yMin - m_axisInterval->yScale());
    m_axisInterval->setYMax(yMax - m_axisInterval->yScale());
    m_contentView.graphView()->reload();
    return true;
  }
  if (event == Ion::Events::Left) {
    m_axisInterval->setXMin(xMin - m_axisInterval->xScale());
    m_axisInterval->setXMax(xMax - m_axisInterval->xScale());
    m_contentView.graphView()->reload();
    return true;
  }
  if (event == Ion::Events::Right) {
    m_axisInterval->setXMin(xMin + m_axisInterval->xScale());
    m_axisInterval->setXMax(xMax + m_axisInterval->xScale());
    m_contentView.graphView()->reload();
    return true;
  }

  return false;
}

void ZoomParameterController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

/* Content View */

ZoomParameterController::ContentView::ContentView(GraphView * graphView) :
  m_graphView(graphView)
{
}

int ZoomParameterController::ContentView::numberOfSubviews() const {
  return 2;
}

View * ZoomParameterController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_graphView;
  }
  return &m_legendView;
}

void ZoomParameterController::ContentView::layoutSubviews() {
  m_graphView->setFrame(bounds());
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight));
}

GraphView * ZoomParameterController::ContentView::graphView() {
  return m_graphView;
}

/* Legend View */

ZoomParameterController::ContentView::LegendView::LegendView()
{
  m_legends[0].setText("ZOOM+");
  m_legends[1].setText("HAUT");
  m_legends[2].setText("GAUCHE");
  m_legends[3].setText("ZOOM-");
  m_legends[4].setText("BAS");
  m_legends[5].setText("DROITE");
  for (int row = 0; row < 3; row++) {
    m_legends[row].setAlignment(0.0f, 0.5f);
    m_legends[3+row].setAlignment(1.0f, 0.5f);
    m_legends[row].setBackgroundColor(k_legendBackgroundColor);
    m_legends[3+row].setBackgroundColor(k_legendBackgroundColor);
  }
}

void ZoomParameterController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), k_legendBackgroundColor);
}

int ZoomParameterController::ContentView::LegendView::numberOfSubviews() const {
  return 6;
}

View * ZoomParameterController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 6);
  return &m_legends[index];
}

void ZoomParameterController::ContentView::LegendView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  for (int row = 0; row < 3; row++) {
    m_legends[row].setFrame(KDRect(k_tokenWidth, row*heigth/3, width/2 - k_tokenWidth, heigth/3));
    m_legends[3+row].setFrame(KDRect(width/2, row*heigth/3, width/2 - k_tokenWidth, heigth/3));
  }
}

}