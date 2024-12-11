#ifndef POINCARE_GHOST_H
#define POINCARE_GHOST_H

#include "ghost_node.h"
#include "pool.h"
#include "pool_handle.h"

namespace Poincare {

/* Ghost is not in ghost_node.h because GhostNode is needed in
 * pool.h and this created header inclusion problems. */

class Ghost final : public PoolHandle {
 public:
  static Ghost Builder() {
    PoolHandle h = PoolHandle::Builder<GhostNode>();
    return static_cast<Ghost &>(h);
  }
};

}  // namespace Poincare

#endif
