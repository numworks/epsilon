#include <omg/utf8_helper.h>
#include <poincare/expression.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/user_expression.h>

namespace Poincare {

using namespace Internal;

bool SymbolHelper::IsTheta(UserExpression e) {
  return e.isUserSymbol() &&
         BuiltinsAliases::k_thetaAliases.contains(GetName(e));
}

bool SymbolHelper::IsSymbol(Expression e, CodePoint c) {
  if (!e.isUserSymbol()) {
    return false;
  }
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  [[maybe_unused]] size_t codePointLength =
      UTF8Helper::WriteCodePoint(buffer, bufferSize - 1, c);
  assert(codePointLength < bufferSize);
  return Internal::Symbol::IsNamed(e, buffer);
}

const char* SymbolHelper::GetName(Expression e) {
  return Internal::Symbol::GetName(e.tree());
}

UserExpression SymbolHelper::BuildSymbol(const char* name, int length) {
  if (length < 0) {
    length = strlen(name);
  }
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
    length = strlen(name);
  }
  return UserExpression::Builder(
      SharedTreeStack->pushUserSymbol(name, static_cast<size_t>(length + 1)));
}

UserExpression SymbolHelper::BuildSymbol(CodePoint name) {
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointLength =
      UTF8Helper::WriteCodePoint(buffer, bufferSize - 1, name);
  assert(codePointLength < bufferSize);
  return BuildSymbol(buffer, codePointLength);
}

UserExpression SymbolHelper::BuildFunction(const char* name,
                                           UserExpression child) {
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
  }
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserFunction(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  return UserExpression::Builder(e);
}

UserExpression SymbolHelper::BuildSequence(const char* name,
                                           UserExpression child) {
  // If needed, handle theta like functions and symbols
  assert(!BuiltinsAliases::k_thetaAliases.contains(name));
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserSequence(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  return UserExpression::Builder(e);
}

}  // namespace Poincare
