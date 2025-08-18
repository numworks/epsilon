#ifndef POINCARE_ABSTRACT_SYMBOL_H
#define POINCARE_ABSTRACT_SYMBOL_H

#include "junior_expression.h"
#include "old_expression.h"

namespace Poincare {

/* TODO: should we keep the size of SymbolAbstractNode as a member to speed up
 * Pool scan? */

/* SymbolAbstract derived classes must have a char[0] member variable as their
 * last member variable, so they can access their name, which is the string that
 * follows the node in memory.
 * This means that a DerivedSymbolNode's size is sizeof(DerivedSymbolNode) +
 * strlen(string).
 *
 * For instance:
 *   Seen by Pool:    |SymbolNode                               |
 *   SymbolNode layout:   |ExpressionNode|m_name|                   |
 *   Memory content:      |ExpressionNode|S     |y|m|b|o|l|N|a|m|e|0|
 * */

class SymbolAbstractNode : public ExpressionNode {
 public:
  SymbolAbstractNode(const char *newName, int length);

  const char *name() const { return m_name; }

  size_t size() const override;

  // ExpressionNode
  int simplificationOrderSameType(const ExpressionNode *e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Property
  OMG::Troolean isPositive(Context *context) const override;
  OExpression replaceSymbolWithExpression(
      const SymbolAbstract &symbol, const OExpression &expression) override;
  ExpressionNode::LayoutShape leftLayoutShape() const override;

  // PoolObject
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override {
    stream << "SymbolAbstract";
  }
  void logAttributes(std::ostream &stream) const override {
    stream << " name=\"" << name() << "\"";
  }
#endif

 protected:
  // Layout
  size_t serialize(char *buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  char m_name[0];  // MUST be the last member variable

 private:
  virtual size_t nodeSize() const = 0;
};

/* WARNING: SymbolAbstract cannot have any virtual methods. Otherwise,
 * inheriting OExpression won't fulfil the requirement:
 * 'sizeof(OExpression) == sizeof(ExpressionInheritingFromSymbolAbstract)
 * due to the virtual table. */

class SymbolAbstract : public Expression {
  friend class Function;
  friend class FunctionNode;
  friend class Sequence;
  friend class SequenceNode;
  friend class Symbol;
  friend class SymbolNode;
  friend class SymbolAbstractNode;
  friend class SumAndProductNode;

 public:
  const char *name() const;
  bool hasSameNameAs(const SymbolAbstract &other) const;
  // Implemented in Expression::replaceSymbolWithExpression
#if 0
  Expression replaceSymbolWithExpression(
      const SymbolAbstract &symbol, const Expression &expression);
#endif

 protected:
  SymbolAbstract(const SymbolAbstractNode *node) : Expression(node) {}
  SymbolAbstractNode *node() const {
    // TODO_PCJ
    assert(false);
    return nullptr;
    // return static_cast<SymbolAbstractNode *>(Expression::node());
  }

 private:
  static Expression Expand(const SymbolAbstract &symbol, Context *context,
                           bool clone, SymbolicComputation symbolicComputation);
};

}  // namespace Poincare

#endif
