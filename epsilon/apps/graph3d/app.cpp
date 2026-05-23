#include "app.h"
#include "apps/graph3d/graph3d_icon.h"
#include "apps/graph/graph_icon.h"
#include <apps/apps_container.h>
#include <apps/shared/global_store.h>
#include <apps/global_preferences.h>
#include <poincare/preferences.h>
#include <poincare/expression.h>
#include <poincare/system_expression.h>
#include <poincare/user_expression.h>
#include <poincare/pool_variable_context.h>
#include <poincare/layout.h>
#include <poincare/print.h>
#include <cmath>

namespace Graph3d {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::Graph3dApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::Graph3dAppCapital;
}

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::GraphIcon;
}

App::Snapshot::Snapshot() : Shared::SharedApp::Snapshot(), m_gridNeedsUpdate(true) {
  strlcpy(m_surfaceExpression, "cos(x)*sin(y)", sizeof(m_surfaceExpression));
}

App * App::Snapshot::unpack(Escher::Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor * App::Snapshot::descriptor() const {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
  m_gridNeedsUpdate = true;
}
void App::Snapshot::tidy() {}

App::InputController::InputController(Escher::Responder * parentResponder, App * app) :
  ViewController(parentResponder),
  m_app(app),
  m_contentView(this, this) {}

void App::InputController::handleResponderChainEvent(ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(m_contentView.layoutField());
    m_contentView.layoutField()->setEditing(true);

    // Set initial text
    Poincare::Layout l = Poincare::UserExpression::Parse(m_app->snapshot()->m_surfaceExpression, Shared::GlobalContextAccessor::Context()).createLayout(Poincare::Preferences::PrintFloatMode::Decimal, 7, Shared::GlobalContextAccessor::Context());
    if (!l.isUninitialized()) {
      m_contentView.layoutField()->setLayout(l);
    }
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

bool App::InputController::layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) {
  return false;
}

bool App::InputController::layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Ion::Events::Event event) {
  Poincare::Layout l = layoutField->layout();
  char buffer[256];
  l.serialize(buffer);
  strlcpy(m_app->snapshot()->m_surfaceExpression, buffer, sizeof(m_app->snapshot()->m_surfaceExpression));

  m_app->snapshot()->m_gridNeedsUpdate = true;
  m_app->openMainView();
  return true;
}

void App::InputController::layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) {}

void App::InputController::layoutFieldDidChangeSize(Escher::LayoutField * layoutField) {
  m_contentView.reload();
}

App::App(Snapshot * snapshot) :
  Shared::SharedApp(snapshot, &m_inputController),
  m_mainViewController(this, this),
  m_inputController(this, this)
{
}

void App::openMainView() {
  if (snapshot()->m_gridNeedsUpdate) {
    recalculateGrid();
  }
  App::app()->displayModalViewController(&m_mainViewController, 0.0f, 0.0f);
}

void App::recalculateGrid() {
  float xMin = -5.0f, xMax = 5.0f;
  float yMin = -5.0f, yMax = 5.0f;
  float dx = (xMax - xMin) / k_gridSize;
  float dy = (yMax - yMin) / k_gridSize;

  const Poincare::SymbolContext & globalContext = Shared::GlobalContextAccessor::Context();
  Poincare::UserExpression e = Poincare::UserExpression::Parse(snapshot()->m_surfaceExpression, globalContext);

  Poincare::AngleUnit angleUnit = GlobalPreferences::SharedGlobalPreferences()->angleUnit();
  Poincare::ComplexFormat complexFormat = GlobalPreferences::SharedGlobalPreferences()->complexFormat();

  if (!e.isUninitialized()) {
    for (int i = 0; i <= k_gridSize; ++i) {
      for (int j = 0; j <= k_gridSize; ++j) {
        float x = xMin + i * dx;
        float y = yMin + j * dy;

        char bufferX[32];
        char bufferY[32];
        Poincare::Print::CustomPrintf(bufferX, sizeof(bufferX), "%*.*ef", x, Poincare::Preferences::PrintFloatMode::Decimal, 7);
        Poincare::Print::CustomPrintf(bufferY, sizeof(bufferY), "%*.*ef", y, Poincare::Preferences::PrintFloatMode::Decimal, 7);

        Poincare::UserExpression xExprFromStr = Poincare::UserExpression::Parse(bufferX, globalContext);
        Poincare::UserExpression yExprFromStr = Poincare::UserExpression::Parse(bufferY, globalContext);

        Poincare::PoolVariableContext context1("x", xExprFromStr, &globalContext);
        Poincare::PoolVariableContext context2("y", yExprFromStr, &context1);

        float z = e.approximateToRealScalar<float>(angleUnit, complexFormat, context2);
        snapshot()->m_zGrid[i][j] = z;
      }
    }
  } else {
    for (int i = 0; i <= k_gridSize; ++i) {
      for (int j = 0; j <= k_gridSize; ++j) {
        snapshot()->m_zGrid[i][j] = NAN;
      }
    }
  }

  snapshot()->m_gridNeedsUpdate = false;
}


bool App::MainViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_view.setRotation(0, 0.1f);
    return true;
  } else if (event == Ion::Events::Down) {
    m_view.setRotation(0, -0.1f);
    return true;
  } else if (event == Ion::Events::Left) {
    m_view.setRotation(-0.1f, 0);
    return true;
  } else if (event == Ion::Events::Right) {
    m_view.setRotation(0.1f, 0);
    return true;
  } else if (event == Ion::Events::Back || event == Ion::Events::Home) {
    App::app()->modalViewController()->dismissModal();
    return true;
  }
  return false;
}

// Fills a triangle utilizing standard barycentric rasterization technique to mimic solid surfaces
void fillTriangle(KDContext * ctx, float x1, float y1, float x2, float y2, float x3, float y3, KDColor color) {
  int minX = std::max(0, (int)std::min({x1, x2, x3}));
  int maxX = std::min(319, (int)std::max({x1, x2, x3}));
  int minY = std::max(0, (int)std::min({y1, y2, y3}));
  int maxY = std::min(239, (int)std::max({y1, y2, y3}));

  float denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
  if (std::abs(denominator) < 0.1f) return;

  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      float lambda1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator;
      float lambda2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator;
      float lambda3 = 1.0f - lambda1 - lambda2;

      if (lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0) {
        ctx->setPixel(KDPoint(x, y), color);
      }
    }
  }
}

void App::MainView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorBlack);

  float xMin = -5.0f, xMax = 5.0f;
  float yMin = -5.0f, yMax = 5.0f;
  float dx = (xMax - xMin) / k_gridSize;
  float dy = (yMax - yMin) / k_gridSize;

  // Rotation matrices setup
  float cosT = std::cos(m_theta);
  float sinT = std::sin(m_theta);
  float cosP = std::cos(m_phi);
  float sinP = std::sin(m_phi);

  auto project = [&](float x, float y, float z, float &px, float &py) {
    float x1 = x * cosT - y * sinT;
    float y1 = x * sinT + y * cosT;
    float z1 = z;

    float y2 = y1 * cosP - z1 * sinP;

    float scale = 25.0f;
    px = 160.0f + x1 * scale;
    py = 120.0f - y2 * scale; // Y goes down on screen
  };

  // Draw filled surface (instead of basic wireframe points)
  for (int i = 0; i < k_gridSize; ++i) {
    for (int j = 0; j < k_gridSize; ++j) {
      float z1 = m_app->snapshot()->m_zGrid[i][j];
      float z2 = m_app->snapshot()->m_zGrid[i+1][j];
      float z3 = m_app->snapshot()->m_zGrid[i][j+1];
      float z4 = m_app->snapshot()->m_zGrid[i+1][j+1];

      if (std::isnan(z1) || std::isnan(z2) || std::isnan(z3) || std::isnan(z4)) continue;

      float px1, py1, px2, py2, px3, py3, px4, py4;
      project(xMin + i*dx, yMin + j*dy, z1, px1, py1);
      project(xMin + (i+1)*dx, yMin + j*dy, z2, px2, py2);
      project(xMin + i*dx, yMin + (j+1)*dy, z3, px3, py3);
      project(xMin + (i+1)*dx, yMin + (j+1)*dy, z4, px4, py4);

      // Create a gradient-like pseudo shading based on Z-height to make the shape continuous and distinguishable like GeoGebra
      uint8_t cIntensity = std::max(50.0f, std::min(255.0f, 150.0f + z1 * 20.0f));
      KDColor color = KDColor::RGB24(0, cIntensity, cIntensity);

      fillTriangle(ctx, px1, py1, px2, py2, px3, py3, color);
      fillTriangle(ctx, px2, py2, px3, py3, px4, py4, color);
    }
  }

  // Draw axes
  float ax_x, ax_y, ax_x2, ax_y2;
  auto drawLine = [&](float x0, float y0, float x1, float y1, KDColor c) {
    int dx = std::round(x1 - x0);
    int dy = std::round(y1 - y0);
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int steps = adx > ady ? adx : ady;
    if (steps == 0) return;
    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;
    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; i++) {
      if (x >= 0 && x < 320 && y >= 0 && y < 240) {
        ctx->setPixel(KDPoint((int)x, (int)y), c);
      }
      x += xInc;
      y += yInc;
    }
  };

  project(0, 0, 0, ax_x, ax_y);
  project(5, 0, 0, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorRed); // X
  project(0, 5, 0, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorGreen); // Y
  project(0, 0, 5, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorBlue); // Z

  ctx->drawString(m_app->snapshot()->m_surfaceExpression, KDPoint(5, 5), {.glyphColor = KDColorWhite, .backgroundColor = KDColorBlack, .font = KDFont::Size::Large});
}

}
