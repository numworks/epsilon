#ifndef POINCARE_ABSTRACT_SYMBOL_H
#define POINCARE_ABSTRACT_SYMBOL_H

#include <poincare/expression.h>

namespace Poincare {

/* TODO: should we keep the size of SymbolAbstractNode as a member to speed up
 * TreePool scan? */

/* SymbolAbstract derived classes must have a char[0] member variable as their
 * last member variable, so they can access their name, which is the string that
 * follows the node in memory.
 * This means that a DerivedSymbolNode's size is sizeof(DerivedSymbolNode) +
 * strlen(string).
 *
 * For instance:
 *   Seen by TreePool:    |SymbolNode                               |
 *   SymbolNode layout:   |ExpressionNode|m_name|                   |
 *   Memory content:      |ExpressionNode|S     |y|m|b|o|l|N|a|m|e|0|
 * */

class SymbolAbstractNode : public ExpressionNode {
  friend class Store;
public:
  virtual const char * name() const = 0;
  void setName(const char * newName, int length);

  // ExpressionNode
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

  // TreeNode
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "SymbolAbstract";
  }
#endif

protected:
  void initName(size_t nameSize);
};

class SymbolAbstract : public Expression {
public:
  const char * name() const { return node()->name(); }
protected:
  SymbolAbstract(const SymbolAbstractNode * node) : Expression(node) {}
  SymbolAbstractNode * node() const { return static_cast<SymbolAbstractNode *>(Expression::node()); }
};

}

#endif
