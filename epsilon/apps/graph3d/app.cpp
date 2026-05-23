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
    m_view.setRotation(0, 0.2f);
    return true;
  } else if (event == Ion::Events::Down) {
    m_view.setRotation(0, -0.2f);
    return true;
  } else if (event == Ion::Events::Left) {
    m_view.setRotation(-0.2f, 0);
    return true;
  } else if (event == Ion::Events::Right) {
    m_view.setRotation(0.2f, 0);
    return true;
  } else if (event == Ion::Events::Back || event == Ion::Events::Home) {
    App::app()->modalViewController()->dismissModal();
    return true;
  }
  return false;
}

// Optimized flat-bottom/flat-top triangle filling logic to speed up rendering loops
void fillTriangle(KDContext * ctx, float x1, float y1, float x2, float y2, float x3, float y3, KDColor color) {
  // Sort vertices by y
  if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
  if (y1 > y3) { std::swap(x1, x3); std::swap(y1, y3); }
  if (y2 > y3) { std::swap(x2, x3); std::swap(y2, y3); }

  int minX = std::max(0, (int)std::min({x1, x2, x3}));
  int maxX = std::min(319, (int)std::max({x1, x2, x3}));
  int minY = std::max(0, (int)y1);
  int maxY = std::min(239, (int)y3);

  if (minX > maxX || minY > maxY) return;

  float denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
  if (std::abs(denom) < 0.1f) return;

  float invDenom = 1.0f / denom;

  for (int y = minY; y <= maxY; ++y) {
    bool drawing = false;
    int startX = minX;

    // Scanline from left to right finding extents rather than evaluating everything
    for (int x = minX; x <= maxX; ++x) {
      float l1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) * invDenom;
      float l2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) * invDenom;
      float l3 = 1.0f - l1 - l2;

      bool inside = (l1 >= 0 && l2 >= 0 && l3 >= 0);

      if (inside && !drawing) {
        drawing = true;
        startX = x;
      } else if (!inside && drawing) {
        // We exited the triangle, we can draw a fast horizontal line
        ctx->fillRect(KDRect(startX, y, x - startX, 1), color);
        drawing = false;
        break; // Convex shape, we can skip the rest of the scanline
      }
    }

    // In case the triangle touched the right edge
    if (drawing) {
      ctx->fillRect(KDRect(startX, y, maxX - startX + 1, 1), color);
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

  // We map colors statically now depending on absolute Z value, avoiding dynamic intensity problems
  auto colorForZ = [](float z) {
    if (std::isnan(z)) return KDColorBlack;
    // Map z from roughly [-5, 5] into hue bins
    if (z > 3.0f) return KDColorRed;
    if (z > 1.0f) return KDColorOrange;
    if (z > -1.0f) return KDColorYellow;
    if (z > -3.0f) return KDColorGreen;
    return KDColorBlue;
  };

  // Draw axes underneath
  auto drawLine = [&](float x0, float y0, float x1, float y1, KDColor c) {
    int _dx = std::round(x1 - x0);
    int _dy = std::round(y1 - y0);
    int adx = _dx < 0 ? -_dx : _dx;
    int ady = _dy < 0 ? -_dy : _dy;
    int steps = adx > ady ? adx : ady;
    if (steps == 0) return;
    float xInc = _dx / (float)steps;
    float yInc = _dy / (float)steps;
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

  float ax_x, ax_y, ax_x2, ax_y2;

  // X Axis
  project(-6, 0, 0, ax_x, ax_y);
  project(6, 0, 0, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorWhite);
  project(6.5, 0, 0, ax_x2, ax_y2);
  ctx->drawString("X", KDPoint(ax_x2, ax_y2), {.glyphColor = KDColorRed, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});

  // Y Axis
  project(0, -6, 0, ax_x, ax_y);
  project(0, 6, 0, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorWhite);
  project(0, 6.5, 0, ax_x2, ax_y2);
  ctx->drawString("Y", KDPoint(ax_x2, ax_y2), {.glyphColor = KDColorGreen, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});

  // Z Axis
  project(0, 0, -6, ax_x, ax_y);
  project(0, 0, 6, ax_x2, ax_y2);
  drawLine(ax_x, ax_y, ax_x2, ax_y2, KDColorWhite);
  project(0, 0, 6.5, ax_x2, ax_y2);
  ctx->drawString("Z", KDPoint(ax_x2, ax_y2), {.glyphColor = KDColorBlue, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});

  // Draw tick marks and numbers for X, Y, Z axes
  for (int i = -5; i <= 5; i++) {
    if (i == 0) continue; // Skip origin overlapping

    char buf[8];
    Poincare::Print::CustomPrintf(buf, sizeof(buf), "%i", i);

    // X ticks
    float px, py;
    project(i, 0, 0, px, py);
    ctx->drawLine(KDPoint(px, py - 2), KDPoint(px, py + 2), KDColorWhite);
    ctx->drawString(buf, KDPoint(px - 3, py + 3), {.glyphColor = KDColorRed, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});

    // Y ticks
    project(0, i, 0, px, py);
    ctx->drawLine(KDPoint(px - 2, py), KDPoint(px + 2, py), KDColorWhite);
    ctx->drawString(buf, KDPoint(px + 3, py - 3), {.glyphColor = KDColorGreen, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});

    // Z ticks
    project(0, 0, i, px, py);
    ctx->drawLine(KDPoint(px - 2, py), KDPoint(px + 2, py), KDColorWhite);
    ctx->drawString(buf, KDPoint(px + 3, py - 3), {.glyphColor = KDColorBlue, .backgroundColor = KDColorBlack, .font = KDFont::Size::Small});
  }


  // Draw filled surface
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

      KDColor color = colorForZ((z1+z2+z3+z4)/4.0f);

      fillTriangle(ctx, px1, py1, px2, py2, px3, py3, color);
      fillTriangle(ctx, px2, py2, px3, py3, px4, py4, color);

      // Keep wireframe lines to help outline the surface patches visually over the solid blocks
      ctx->drawLine(KDPoint(px1, py1), KDPoint(px2, py2), KDColorBlack);
      ctx->drawLine(KDPoint(px1, py1), KDPoint(px3, py3), KDColorBlack);
    }
  }

  ctx->drawString(m_app->snapshot()->m_surfaceExpression, KDPoint(5, 5), {.glyphColor = KDColorWhite, .backgroundColor = KDColorBlack, .font = KDFont::Size::Large});
}

}
