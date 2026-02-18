/* These tests were originally part of the poincare test suite, but they rely on
 * the application-dependent implementation of the sequence context. */

#include <poincare/src/expression/k_tree.h>
#include <poincare/test/helper.h>
#include <poincare/test/helpers/symbol_store.h>
#include <poincare/test/simplification/helper.h>

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(poincare_sequence) {
  PoincareTest::SymbolStore symbolStore;
  SimplificationParameters simplificationParameters = {
      .symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .context = symbolStore};

  // Sequence
  // TODO: add sequence type to the symbol store
  symbolStore.setExpressionForUserSymbol(KUndef, KSeq<"u">("n"_e));
  PoincareTest::store("3→f(x)", symbolStore);
  // Sequence are kept in dependency
  simplifies_to("f(u(n))", "dep(3,{u(n)})", simplificationParameters);
  // Except if the sequence can already be approximated.
  simplifies_to("f(u(2))", "undef", simplificationParameters);
  symbolStore.reset();
}
