#ifndef CHAR_LAYOUT_H
#define CHAR_LAYOUT_H

#include "layout_reference.h"
#include "layout_node.h"

//#define TREE_LOGGING 1

class CharLayoutNode : public LayoutNode {
public:
  CharLayoutNode() : LayoutNode() {}
  size_t size() const override {
    return sizeof(CharLayoutNode);
  }
#if TREE_LOGGING
  const char * description() const override {
    static char Description[] = {'C', 'h', 'a', 'r', ' ', m_char, 0};
    return Description;
  }
#endif
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
