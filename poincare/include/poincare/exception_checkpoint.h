#ifndef POINCARE_EXCEPTION_CHECKPOINT_H
#define POINCARE_EXCEPTION_CHECKPOINT_H

#include "tree_pool.h"
#include "tree_node.h"
#include <setjmp.h>


/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the ExceptionCheckpoint

void errorCatcher() {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    CodeUsingPoincare();
  } else {
    ErrorHandler();
  }
}

To raise an error : ExceptionCheckpoint::Raise();

*/

#define ExceptionRun(ecp) (setjmp(*(ecp.jumpBuffer())) == 0)

namespace Poincare {

class ExceptionCheckpoint {
public:
  static void Raise() {
    assert(s_topmostExceptionCheckpoint != nullptr);
    s_topmostExceptionCheckpoint->rollback();
  }

  ExceptionCheckpoint();

  ~ExceptionCheckpoint() {
    s_topmostExceptionCheckpoint = m_parent;
  }

  jmp_buf * jumpBuffer() { return &m_jumpBuffer; }

private:
  void rollback();

  static ExceptionCheckpoint * s_topmostExceptionCheckpoint;

  jmp_buf m_jumpBuffer;
  TreeNode * m_endOfPoolBeforeCheckpoint;
  ExceptionCheckpoint * m_parent;
};

}

#endif
