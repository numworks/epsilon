#include <poincare/helpers/symbol.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

using namespace Internal;

const char* SymbolHelper::AnsMainAlias() {
  return BuiltinsAliases::k_ansAliases.mainAlias();
}

const char* SymbolHelper::GetName(const Tree* e) {
  return Internal::Symbol::GetName(e);
}

size_t SymbolHelper::NameWithoutQuotationMarks(char* buffer, size_t bufferSize,
                                               const char* name,
                                               size_t nameLength) {
  if (Symbol::NameHasQuotationMarks(name, nameLength)) {
    assert(bufferSize > nameLength - 2);
    size_t result = strlcpy(buffer, name + 1, bufferSize) - 1;
    buffer[nameLength - 2] = 0;  // Remove the last '""
    return result;
  }
  assert(bufferSize > nameLength);
  return strlcpy(buffer, name, bufferSize);
}

}  // namespace Poincare
