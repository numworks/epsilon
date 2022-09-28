#include "interface.h"
#include "../edition_pool.h"

namespace Poincare {

template <typename T, int N, typename... Types>
TypeBlock * Interface::PushNode(Types... args) {
  Block blocks[N];
  size_t numberOfUsedBlocks = T::CreateNodeAtAddress(blocks, args...);
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock * newNode = static_cast<TypeBlock *>(pool->lastBlock());
  for (size_t i = 0; i < numberOfUsedBlocks; i++) {
    pool->pushBlock(blocks[i]);
  }
  return newNode;
}

}

template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::AdditionInterface, 3, uint8_t>(uint8_t);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::IntegerInterface, 12, int>(int);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::MultiplicationInterface, 3, uint8_t>(uint8_t);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::PowerInterface, 1>();
