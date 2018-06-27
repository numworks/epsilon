#ifndef HORIZONTAL_LAYOUT_H
#define HORIZONTAL_LAYOUT_H

#include "layout_reference.h"
#include "layout_node.h"
#include "layout_cursor.h"

class HorizontalLayoutNode : public LayoutNode {
public:
  HorizontalLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  size_t size() const override {
    return sizeof(HorizontalLayoutNode);
  }

  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren() override { m_numberOfChildren++; }

  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {
    if (this == cursor->layoutReference().node()) {
      if (cursor->position() == LayoutCursor::Position::Left) {
        // Case: Left. Ask the parent.
        LayoutNode * parentNode = parent();
        if (parentNode != nullptr) {
          parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
        }
        return;
      }
      assert(cursor->position() == LayoutCursor::Position::Right);
      /* Case: Right. Go to the last child if there is one, and move Left. Else
       * go Left and ask the parent. */
      int childrenCount = numberOfChildren();
      if (childrenCount >= 1) {
        cursor->setLayoutNode(static_cast<LayoutNode *>(childTreeAtIndex(childrenCount-1)));
      } else {
        cursor->setPosition(LayoutCursor::Position::Left);
      }
      return cursor->moveLeft(shouldRecomputeLayout);
    }

    // Case: The cursor is Left of a child.
    assert(cursor->position() == LayoutCursor::Position::Left);
    int childIndex = indexOfChildByIdentifier(cursor->layoutIdentifier());
    assert(childIndex >= 0);
    if (childIndex == 0) {
      // Case: the child is the leftmost. Ask the parent.
      if (parent()) {
        cursor->setLayoutNode(this);
        return cursor->moveLeft(shouldRecomputeLayout);
      }
      return;
    }
    // Case: the child is not the leftmost. Go to its left sibling and move Left.
    cursor->setLayoutNode(static_cast<LayoutNode *>(childTreeAtIndex(childIndex-1)));
    cursor->setPosition(LayoutCursor::Position::Right);
    cursor->moveLeft(shouldRecomputeLayout);
  }

  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {
    //TODO
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
      parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
    }
  }

  const char * description() const override {
    return "Horizontal Layout";
  }
private:
  int m_numberOfChildren;
};

class HorizontalLayoutRef : public LayoutReference<HorizontalLayoutNode> {
public:
  HorizontalLayoutRef() : LayoutReference<HorizontalLayoutNode>() {}
  HorizontalLayoutRef(LayoutRef l) : LayoutReference<HorizontalLayoutNode>() {
    addChild(l);
  }

  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2) : LayoutReference<HorizontalLayoutNode>() {
    addChild(l2);
    addChild(l1);
  }
};

#endif
