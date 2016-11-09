#ifndef APPS_NODE_H
#define APPS_NODE_H

class Node {
public:
  constexpr Node(const char * label = nullptr, int numberOfChildren = 0) :
    m_label(label),
    m_numberOfChildren(numberOfChildren)
  {
  };
  virtual const Node * children(int index) const = 0;
  const char * label() const;
  int numberOfChildren();
  bool isNull();
protected:
  const char * m_label;
  int m_numberOfChildren;
};

#endif

