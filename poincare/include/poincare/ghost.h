#ifndef POINCARE_GHOST_H
#define POINCARE_GHOST_H

#include <poincare/ghost_node.h>
#include <poincare/tree_handle.h>
#include <poincare/tree_pool.h>

namespace Poincare {

/* Ghost is not in ghost_node.h because GhostNode is needed in
 * tree_pool.h and this created header inclusion problems. */

class Ghost final : public TreeHandle {
public:
  static Ghost Builder() {
    void * bufferNode = TreePool::sharedPool()->alloc(sizeof(GhostNode));
    GhostNode * node = new (bufferNode) GhostNode();
    TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
    return static_cast<Ghost &>(h);
  }
};

}

#endif
