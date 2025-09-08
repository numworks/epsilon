#ifndef POINCARE_TEST_VARIABLE_STORE_H
#define POINCARE_TEST_VARIABLE_STORE_H

#include <omg/string.h>
#include <omg/vector.h>
#include <poincare/src/memory/block.h>
#include <poincare/symbol_store_interface.h>

#include <string_view>

namespace Poincare::Internal {
class Tree;
}  // namespace Poincare::Internal

namespace PoincareTest {

void store(const char* storeExpression,
           Poincare::SymbolStoreInterface& symbolStore);
void store(const char* symbol, const char* expression,
           Poincare::SymbolStoreInterface& symbolStore);

class SymbolStore : public Poincare::SymbolStoreInterface {
 public:
  const Poincare::Internal::Tree* expressionForUserNamed(
      const Poincare::Internal::Tree* symbol) const override;

  UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override;

  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override;

  // Destroy all symbols
  void reset() { m_symbolTable.clear(); }

 private:
  class SymbolWithExpression {
   public:
    SymbolWithExpression(std::string_view name, UserNamedType type,
                         const Poincare::Internal::Tree* e);
    SymbolWithExpression() = default;

    std::string_view name() const { return m_name; }
    UserNamedType type() const { return m_type; }
    const Poincare::Internal::Tree* expression() const;

   private:
    constexpr static size_t k_maxNameSize = 10;
    OMG::String<k_maxNameSize> m_name;
    UserNamedType m_type;
    constexpr static size_t k_expressionBufferSize = 25;
    std::array<Poincare::Internal::Block, k_expressionBufferSize>
        m_expressionBuffer;
  };

  bool setExpressionForUserFunctionOrSequence(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* functionialSymbol);

  bool setExpressionForUserSymbol(const Poincare::Internal::Tree* expression,
                                  std::string_view symbolName);

  bool push(const Poincare::Internal::Tree* expression,
            std::string_view symbolName, UserNamedType symbolType);

  // Returns a nullptr if the symbol does not exist in the store
  const SymbolWithExpression* findSymbolInStore(
      std::string_view symbolName) const;
  SymbolWithExpression* findSymbolInStore(std::string_view symbolName);

  static constexpr size_t k_maxSymbols = 10;
  using SymbolTable = OMG::StaticVector<SymbolWithExpression, k_maxSymbols>;
  SymbolTable m_symbolTable;
};

}  // namespace PoincareTest

#endif
