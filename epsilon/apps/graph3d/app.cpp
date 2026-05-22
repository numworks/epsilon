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

void drawLine(KDContext * ctx, float x0, float y0, float x1, float y1, KDColor c) {
  int dx = std::round(x1 - x0);
  int dy = std::round(y1 - y0);
  int adx = dx < 0 ? -dx : dx;
  int ady = dy < 0 ? -dy : dy;
  int steps = adx > ady ? adx : ady;
  if (steps == 0) {
    return;
  }
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

  // Draw wireframe grid
  for (int i = 0; i <= k_gridSize; ++i) {
    for (int j = 0; j <= k_gridSize; ++j) {
      if (std::isnan(m_app->snapshot()->m_zGrid[i][j])) continue;

      float px, py;
      project(xMin + i*dx, yMin + j*dy, m_app->snapshot()->m_zGrid[i][j], px, py);

      if (i < k_gridSize && !std::isnan(m_app->snapshot()->m_zGrid[i+1][j])) {
        float px_next, py_next;
        project(xMin + (i+1)*dx, yMin + j*dy, m_app->snapshot()->m_zGrid[i+1][j], px_next, py_next);
        drawLine(ctx, px, py, px_next, py_next, KDColorWhite);
      }
      if (j < k_gridSize && !std::isnan(m_app->snapshot()->m_zGrid[i][j+1])) {
        float px_next, py_next;
        project(xMin + i*dx, yMin + (j+1)*dy, m_app->snapshot()->m_zGrid[i][j+1], px_next, py_next);
        drawLine(ctx, px, py, px_next, py_next, KDColorWhite);
      }
    }
  }

  // Draw axes
  float ax_x, ax_y;
  project(0, 0, 0, ax_x, ax_y);
  float ax_x2, ax_y2;
  project(5, 0, 0, ax_x2, ax_y2);
  drawLine(ctx, ax_x, ax_y, ax_x2, ax_y2, KDColorRed); // X
  project(0, 5, 0, ax_x2, ax_y2);
  drawLine(ctx, ax_x, ax_y, ax_x2, ax_y2, KDColorGreen); // Y
  project(0, 0, 5, ax_x2, ax_y2);
  drawLine(ctx, ax_x, ax_y, ax_x2, ax_y2, KDColorBlue); // Z

  ctx->drawString(m_app->snapshot()->m_surfaceExpression, KDPoint(5, 5), {.glyphColor = KDColorWhite, .backgroundColor = KDColorBlack, .font = KDFont::Size::Large});
}

}
