#ifndef POINCARE_MEMORY_ARBITRARY_DATA_H
#define POINCARE_MEMORY_ARBITRARY_DATA_H

#include "tree.h"

namespace Poincare::Internal {

class ArbitraryData {
 public:
  constexpr static size_t Size(const Tree* tree) {
    assert(tree->isArbitrary());
    constexpr size_t header = TypeBlock::NumberOfMetaBlocks(Type::Arbitrary);
    return tree->nodeSize() - header;
  }

  template <typename T>
  constexpr static T Unpack(const Tree* tree) {
    static_assert(std::is_trivially_copyable<T>::value);
    assert(tree->isArbitrary());
    assert(Size(tree) == sizeof(T));
    constexpr size_t header = TypeBlock::NumberOfMetaBlocks(Type::Arbitrary);
    T result;
    std::memcpy(&result, tree->nextNth(header), sizeof(T));
    return result;
  }
};

}  // namespace Poincare::Internal

#endif
