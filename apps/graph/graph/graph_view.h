#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"
#include "../function_store.h"
#include "../evaluate_context.h"

#define GRAPH_VIEW_IS_TILED 0

namespace Graph {

class GraphView : public
#if GRAPH_VIEW_IS_TILED
                  TiledView
#else
                  View
#endif
{
public:
  GraphView(FunctionStore * functionStore);

#if GRAPH_VIEW_IS_TILED
  KDColor * tile() const override;
  KDSize tileSize() const override;
  void drawTile(KDContext * ctx, KDRect rect) const override;
#else
  void drawRect(KDContext * ctx, KDRect rect) const override;
#endif

//  void drawRect(KDRect rect) const override;
  void moveCursorRight();
  void setContext(Context * evaluateContext);
  Context * context() const;
private:
  constexpr static int k_maxNumberOfIterations = 10;
  constexpr static KDCoordinate k_stampSize = 5;
  constexpr static uint8_t k_mask[k_stampSize*k_stampSize] = {
    0xff, 0xfb, 0x73, 0xfb, 0xff,
    0xfb, 0x30, 0x00, 0x30, 0xfb,
    0x73, 0x00, 0x00, 0x00, 0x73,
    0xfb, 0x30, 0x00, 0x30, 0xfb,
    0xff, 0xfb, 0x73, 0xfb, 0xff
  };
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };

  float min(Axis axis) const;
  float max(Axis axis) const;
  KDCoordinate pixelLength(Axis axis) const;

  float pixelToFloat(Axis axis, KDCoordinate p) const;
  KDCoordinate floatToPixel(Axis axis, float f) const;

  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1) const;

  void drawAxes(KDContext * ctx, KDRect rect) const;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, int count, KDColor color) const;
  void drawFunction(KDContext * ctx, KDRect rect) const;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void jointDots(float x, float y, float u, float v, Function * function, int maxNumberOfRecursion, KDContext * ctx, KDColor * workingBuffer) const;
  /* Join two dots with a straight line (changing abscissa halfway). */
  void straightJoinDots(KDCoordinate px, KDCoordinate py, KDCoordinate pu, KDCoordinate pv, Function * function, KDContext * ctx, KDColor * workingBuffer) const;
  /* Stamp centered around (px, py). */
  void stampAtLocation(KDCoordinate px, KDCoordinate py, KDColor color, KDContext * ctx, KDColor * workingBuffer) const;
#if GRAPH_VIEW_IS_TILED
  static constexpr KDCoordinate kTileWidth = 32;
  static constexpr KDCoordinate kTileHeight = 32;
  KDColor m_tile[kTileWidth*kTileHeight];
#endif

  CursorView m_cursorView;
  KDPoint m_cursorPosition;

  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;

  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
