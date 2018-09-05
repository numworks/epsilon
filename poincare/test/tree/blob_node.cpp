#include "blob_node.h"

namespace Poincare {

TreeNode * BlobNode::uninitializedStaticNode() const {
  return UninitializedBlobNode::UninitializedBlobStaticNode();
}

UninitializedBlobNode * UninitializedBlobNode::UninitializedBlobStaticNode() {
  static UninitializedBlobNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
