#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>

namespace Poincare {

const JuniorExpression StoreHelper::Value(const UserExpression& e) {
  assert(!e.isUninitialized() && e.type() == ExpressionNode::Type::Store);
  return e.cloneChildAtIndex(0);
}

const SymbolAbstract StoreHelper::Symbol(const UserExpression& e) {
  assert(!e.isUninitialized() && e.type() == ExpressionNode::Type::Store);
  const JuniorExpression symbol = e.cloneChildAtIndex(1);
  assert(IsUserSymbol(symbol) || IsUserFunction(symbol));
  return static_cast<const SymbolAbstract&>(symbol);
}

bool StoreHelper::PerformStore(Context* context, const UserExpression& e) {
  return StoreValueForSymbol(context, Value(e), Symbol(e));
}

bool StoreHelper::StoreValueForSymbol(Context* context,
                                      const UserExpression& value,
                                      const SymbolAbstract& symbol) {
  assert(!value.isUninitialized());
  assert(IsUserSymbol(symbol) || IsUserFunction(symbol));
  // TODO_PCJ handle unit store (remove the if once done)
  if (Internal::Dimension::Get(value.tree(), context).isUnit()) {
    return false;
  }
  return context->setExpressionForSymbolAbstract(value, symbol);
}

}  // namespace Poincare
