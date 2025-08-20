#ifndef POINCARE_TEST_VARIABLE_STORE_H
#define POINCARE_TEST_VARIABLE_STORE_H

#include <omg/vector.h>
#include <poincare/src/memory/block.h>
#include <poincare/variable_store.h>

namespace Poincare::Internal {
class Tree;
}  // namespace Poincare::Internal

namespace PoincareTest {

void store(const char* storeExpression, Poincare::VariableStore& variableStore);

class SymbolStore : public Poincare::VariableStore {
 public:
  const Poincare::Internal::Tree* expressionForUserNamed(
      const Poincare::Internal::Tree* symbol) const override;

  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override;

  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override;

 private:
  class SymbolWithExpression {
   public:
    SymbolWithExpression() = default;
    SymbolWithExpression(char name, UserNamedType type,
                         const Poincare::Internal::Tree* e);

    char name() const { return m_name; }
    UserNamedType type() const { return m_type; }
    const Poincare::Internal::Tree* expression() const;

   private:
    char m_name;
    UserNamedType m_type;
    static constexpr size_t k_expressionBufferSize = 25;
    std::array<Poincare::Internal::Block, k_expressionBufferSize>
        m_expressionBuffer;
  };

  bool setExpressionForUserFunction(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* functionSymbol);

  bool push(const Poincare::Internal::Tree* expression, char symbolName,
            UserNamedType symbolType);

  static constexpr size_t k_maxSymbols = 10;
  using SymbolTable = OMG::StaticVector<SymbolWithExpression, k_maxSymbols>;
  SymbolTable m_symbolTable;
};

}  // namespace PoincareTest

#endif
