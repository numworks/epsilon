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
  constexpr static T Unpack(const Tree* tree, size_t offset = 0) {
    static_assert(std::is_trivially_copyable<T>::value);
    assert(tree->isArbitrary());
    assert(Size(tree) >= sizeof(T) + offset);
    constexpr size_t header = TypeBlock::NumberOfMetaBlocks(Type::Arbitrary);
    T result;
    std::memcpy(&result, tree->nextNth(header + offset), sizeof(T));
    return result;
  }

  static void Write(Tree* tree, const void* data, size_t size,
                    size_t offset = 0) {
    assert(tree->isArbitrary());
    assert(Size(tree) >= size + offset);
    constexpr size_t header = TypeBlock::NumberOfMetaBlocks(Type::Arbitrary);
    std::memcpy(tree->nextNth(header + offset), data, size);
  }

  template <typename T>
  static void Write(Tree* tree, const T& data, size_t offset = 0) {
    static_assert(std::is_trivially_copyable<T>::value);
    Write(tree, &data, sizeof(T), offset);
  }
};

}  // namespace Poincare::Internal

#endif
