#ifndef GRAPH_INTEREST_VIEW_H
#define GRAPH_INTEREST_VIEW_H

#include <apps/shared/dots.h>
#include <escher/view.h>
#include <poincare/solver.h>

namespace Graph {

class GraphView;

/* Interest view is distinct from Graph view so that it is drawn after
 * the other subviews (cursor and banner) and it owns another dirtyRect */
class InterestView : public Escher::View {
  friend class GraphView;

 public:
  InterestView(GraphView* parentView)
      : Escher::View(), m_parentView(parentView) {}
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  void dirtyBounds() { markRectAsDirty(bounds()); }
  GraphView* m_parentView;
};

}  // namespace Graph

#endif
