#ifndef GRAPH_INTEREST_VIEW_H
#define GRAPH_INTEREST_VIEW_H

#include <escher/view.h>
#include <poincare/solver.h>

namespace Graph {

class GraphView;

class InterestView : public Escher::View {
public:
  InterestView(GraphView * parentView) : Escher::View(), m_interest(Poincare::Solver<double>::Interest::None), m_parentView(parentView) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setInterest(Poincare::Solver<double>::Interest interest) { m_interest = interest; }
private:
  Poincare::Solver<double>::Interest m_interest;
  GraphView * m_parentView;
};

}

#endif
