#include "sequence_toolbox_data_source.h"

#include <apps/global_preferences.h>
#include <apps/shared/sequence_store.h>
#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

using namespace Poincare;
using namespace Escher;

namespace Sequence {

void SequenceToolboxDataSource::buildExtraCellsLayouts(const char *sequenceName,
                                                       int order) {
  /* If recurrenceDepth < 0, the user is setting the initial conditions so we
   * do not want to add any cell in the toolbox. */
  if (order < 0) {
    m_numberOfAddedCells = 0;
    return;
  }
  /* The cells added represent the sequence at smaller ranks than its depth
   * and the other sequence at ranks smaller or equal to the depth, ie:
   * if the sequence is u(n+1), we add cells u(n), v(n), v(n+1), w(n), w(n+1).
   * There is a special case for double recurrent sequences because we do not
   * want to parse symbols u(n+2), v(n+2) or w(n+2). */
  m_numberOfAddedCells = 0;
  int sequenceIndex =
      Shared::SequenceStore::SequenceIndexForName(sequenceName[0]);
  for (int i = 0; i < Shared::SequenceStore::k_maxNumberOfSequences; i++) {
    for (int j = 0; j < order + 1; j++) {
      // When defining u(n+1) for ex, don't add [u|v|w](n+2) or u(n+1)
      if (j == 2 || (j == order && sequenceIndex == i)) {
        continue;
      }
      const char *indice = j == 0 ? "n" : "n+1";
      assert(m_numberOfAddedCells < k_maxNumberOfLayouts);
      m_addedCellLayout[m_numberOfAddedCells++] = Layout::Create(
          KA ^ KSubscriptL(KB),
          {.KA =
               Layout::CodePoint(Shared::SequenceStore::k_sequenceNames[i][0]),
           .KB = Layout::String(indice)});
    }
  }
}

}  // namespace Sequence
