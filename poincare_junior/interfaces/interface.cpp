#include "interface.h"
#include "../edition_pool.h"

namespace Poincare {

template <typename T, typename... Types>
TypeBlock * Interface::PushNode(Types... args) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock * newNode = static_cast<TypeBlock *>(pool->lastBlock());
  Block block;
  size_t i = 0;
  bool endOfNode = false;
  do {
    endOfNode = T::CreateBlockAtIndex(&block, i++, args...);
    pool->pushBlock(block);
  } while (!endOfNode);
  return newNode;
}

}

template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::AdditionInterface, uint8_t>(uint8_t);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::DivisionInterface>();
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::IntegerInterface, int>(int);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::MultiplicationInterface, uint8_t>(uint8_t);
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::PowerInterface>();
template Poincare::TypeBlock* Poincare::Interface::PushNode<Poincare::SubtractionInterface>();
