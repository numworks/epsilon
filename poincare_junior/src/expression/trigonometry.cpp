#include "trigonometry.h"

namespace PoincareJ {

// TODO: tests

bool Trigonometry::IsDirect(const Node* node) {
  BlockType type = node.type();
  switch (type) {
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
      return true;
    default:
      return false;
  }
}

bool Trigonometry::IsInverse(const Node* node) {
  BlockType type = node.type();
  switch (type) {
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      return true;
    default:
      return false;
  }
}

}  // namespace PoincareJ
