#ifndef CHAR_LAYOUT_H
#define CHAR_LAYOUT_H

#include "layout_reference.h"
#include "layout_node.h"
#include "layout_cursor.h"

class CharLayoutNode : public LayoutNode {
public:
  CharLayoutNode() : LayoutNode() {}
  size_t size() const override {
    return sizeof(CharLayoutNode);
  }

  int numberOfChildren() const override { return 0; }

  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {
    if (cursor->position() == LayoutCursor::Position::Right) {
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
      parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
    }
  }

  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {
    if (cursor->position() == LayoutCursor::Position::Left) {
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
    }
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
      parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
    }
  }

  const char * description() const override {
    static char Description[] = {'C', 'h', 'a', 'r', ' ', m_char, 0};
    return Description;
  }

  void setChar(char c) { m_char = c; }
private:
  char m_char;
};

class CharLayout : public LayoutReference<CharLayoutNode> {
public:
  CharLayout(char c) : LayoutReference<CharLayoutNode>() {
    this->node()->setChar(c);
  }
};

#endif
