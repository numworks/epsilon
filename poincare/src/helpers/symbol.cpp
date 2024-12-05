#include <poincare/helpers/symbol.h>
#include <poincare/src/expression/builtin.h>

namespace Poincare {

using namespace Internal;

const char* SymbolHelper::AnsMainAlias() {
  return BuiltinsAliases::k_ansAliases.mainAlias();
}

}  // namespace Poincare
