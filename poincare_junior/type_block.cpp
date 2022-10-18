#include "type_block.h"

namespace Poincare {

bool TypeBlock::isOfType(std::initializer_list<BlockType> types) const {
  BlockType thisType = type();
  for (BlockType t : types) {
    if (thisType == t) {
      return true;
    }
  }
  return false;
}

}
