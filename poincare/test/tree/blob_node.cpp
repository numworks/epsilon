#include "blob_node.h"

namespace Poincare {

BlobNode * BlobNode::FailedAllocationStaticNode() {
  static AllocationFailureBlobNode failureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
  return &failureNode;
}

}
