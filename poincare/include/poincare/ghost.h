#ifndef POINCARE_GHOST_H
#define POINCARE_GHOST_H

#include <poincare/ghost_node.h>
#include <poincare/pool.h>
#include <poincare/tree_handle.h>

namespace Poincare {

/* Ghost is not in ghost_node.h because GhostNode is needed in
 * pool.h and this created header inclusion problems. */

class Ghost final : public TreeHandle {
 public:
  static Ghost Builder() {
    return TreeHandle::FixedArityBuilder<Ghost, GhostNode>();
  }
};

}  // namespace Poincare

#endif
