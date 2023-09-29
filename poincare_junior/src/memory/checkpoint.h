#ifndef POINCARE_MEMORY_CHECKPOINT_H
#define POINCARE_MEMORY_CHECKPOINT_H

#include "edition_pool.h"

namespace PoincareJ {

class Checkpoint {
 public:
 protected:
  void rollback() { SharedEditionPool->flush(); }
};

}  // namespace PoincareJ

#endif
