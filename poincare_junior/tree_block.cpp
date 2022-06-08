#include "tree_block.h"

namespace Poincare {

const char * TreeBlock::log() {
  BlockType type = static_cast<BlockType>(m_content);
  switch (type) {
    case BlockType::Addition:
      return "Addition";
    case BlockType::Multiplication:
      return "Multiplication";
    case BlockType::Integer:
      return "Integer";
    default:
      static char s_buffer[2] = "0";
      s_buffer[0] = '0'+ m_content;
      return s_buffer;
  }
}

int TreeBlock::numberOfSubtrees() const {
  BlockType type = static_cast<BlockType>(m_content);
  switch (type) {
    case BlockType::Addition:
    case BlockType::Multiplication:
      return 2;
    case BlockType::Integer:
      return 1;
    default:
      return 0;
  }
}

TreeBlock * TreeBlock::nextTree() {
  int nbOfSubtreesToScan = numberOfSubtrees();
  TreeBlock * result = this;
  while (nbOfSubtreesToScan > 0) {
    result = result->nextBlock();
    nbOfSubtreesToScan += result->numberOfSubtrees() - 1;
  }
  return result->nextBlock();
}

}
