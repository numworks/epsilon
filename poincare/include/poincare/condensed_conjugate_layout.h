#ifndef POINCARE_CONDENSED_CONJUGATE_LAYOUT_H
#define POINCARE_CONDENSED_CONJUGATE_LAYOUT_H

#include <poincare/conjugate_layout.h>

namespace Poincare {

/* This class is a ConjugateLayoutNode from which we removed the margins on both side. */
class CondensedConjugateLayoutNode : public ConjugateLayoutNode {
public:
  using ConjugateLayoutNode::ConjugateLayoutNode;

  Type type() const override { return Type::CondensedConjugateLayout; }
  size_t size() const override { return sizeof(ConjugateLayoutNode); }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "CondensedConjugateLayout"; }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDPoint positionOfChild(LayoutNode * child) override;

private:
  void render(KDContext * ctx,
              KDPoint p,
              KDColor expressionColor,
              KDColor backgroundColor,
              Layout * selectionStart = nullptr,
              Layout * selectionEnd = nullptr,
              KDColor selectionColor = KDColorRed) override;
};

class CondensedConjugateLayout final : public Layout {
public:
  static CondensedConjugateLayout Builder(Layout child) {
    return TreeHandle::FixedArityBuilder<CondensedConjugateLayout, CondensedConjugateLayoutNode>(
        {child});
  }
  CondensedConjugateLayout() = delete;
};

}  // namespace Poincare

#endif /* POINCARE_CONDENSED_CONJUGATE_LAYOUT_H */
