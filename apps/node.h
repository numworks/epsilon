#ifndef APPS_NODE_H
#define APPS_NODE_H

class Node {
public:
  constexpr Node(const char * label = nullptr, const char * text = nullptr, const Node * children = nullptr, int numberOfChildren = 0) :
    m_label(label),
    m_text(text),
    m_children(children),
    m_numberOfChildren(numberOfChildren)
  {
  };
  const Node * children(int index) const;
  const char * label() const;
  const char * text() const;
  int numberOfChildren();
  bool isNull();
private:
  const char * m_label;
  const char * m_text;
  const Node * m_children;
  int m_numberOfChildren;
};

#endif

