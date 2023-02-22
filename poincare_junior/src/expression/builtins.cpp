#include "builtins.h"

namespace PoincareJ {

//TODO: aliases

const char * Builtins::Name(const Node node) {
  BlockType type = node.type();
  switch (type) {
    case BlockType::Cosine:
      return "cos";
    case BlockType::Sine:
      return "sin";
    case BlockType::Tangent:
      return "tan";
    case BlockType::ArcCosine:
      return "arccos";
    case BlockType::ArcSine:
      return "arcsin";
    case BlockType::ArcTangent:
      return "arctan";
    default:
      assert(false);
      return "";
  }
}

}
