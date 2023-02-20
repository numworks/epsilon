#ifndef POINCARE_ABSTRACT_SYMBOL_H
#define POINCARE_ABSTRACT_SYMBOL_H

#include <poincare/expression.h>

namespace Poincare {

/* TODO: should Function directly inherit from Symbol and we remove
 * SymbolAbstract? */

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
 public:
  /* A symbol abstract can have a max length of 7 chars, or 9 if it's
   * surrounded by quotation marks.
   * This makes it so a 9 chars name (with quotation marks), can be
   * turned into a 7 char name in the result cells of the solver (by
   * removing the quotation marks). */
  constexpr static size_t k_maxNameLengthWithoutQuotationMarks = 7;
  constexpr static size_t k_maxNameLength =
      k_maxNameLengthWithoutQuotationMarks + 2;
  constexpr static size_t k_maxNameSize = k_maxNameLength + 1;
  static bool NameHasQuotationMarks(const char *name, size_t length) {
    return length > 2 && name[0] == '"' && name[length - 1] == '"';
  }
  static bool NameLengthIsValid(const char *name, size_t length) {
    return length <= k_maxNameLengthWithoutQuotationMarks ||
           (NameHasQuotationMarks(name, length) && length <= k_maxNameLength);
  }
  static size_t NameWithoutQuotationMarks(char *buffer, size_t bufferSize,
                                          const char *name, size_t nameLength);

  virtual const char *name() const = 0;

  size_t size() const override;

  // ExpressionNode
  int simplificationOrderSameType(const ExpressionNode *e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Property
  TrinaryBoolean isPositive(Context *context) const override;

  // TreeNode
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
  int serialize(char *buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  // editableName is used in setName
  virtual char *editableName() {
    assert(false);
    return nullptr;
  }
  void setName(const char *name, size_t length);

 private:
  virtual size_t nodeSize() const = 0;
};

/* WARNING: symbol abstract cannot have any virtual methods. Otherwise,
 * inheriting Expression won't fulfil the requirement:
 * 'sizeof(Expression) == sizeof(ExpressionInheritingFromSymbolAbstract)
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
  const char *name() const { return node()->name(); }
  bool hasSameNameAs(const SymbolAbstract &other) const;
  static size_t TruncateExtension(char *dst, const char *src, size_t size);
  static bool matches(const SymbolAbstract &symbol, ExpressionTrinaryTest test,
                      Context *context, void *auxiliary = nullptr);
  constexpr static size_t k_maxNameSize = 8;

 protected:
  SymbolAbstract(const SymbolAbstractNode *node) : Expression(node) {}
  template <typename T, typename U>
  static T Builder(const char *name, int length);
  SymbolAbstractNode *node() const {
    return static_cast<SymbolAbstractNode *>(Expression::node());
  }

 private:
  static Expression Expand(const SymbolAbstract &symbol, Context *context,
                           bool clone, SymbolicComputation symbolicComputation);
};

}  // namespace Poincare

#endif
