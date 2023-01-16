#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <quiz.h>

using namespace PoincareJ;

inline Node createSimpleExpression() {
  Node multiplication = EditionReference::Push<BlockType::Multiplication>(3);
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(1);
  EditionReference::Push<BlockType::IntegerShort>(2);
  EditionReference::Push<BlockType::IntegerShort>(3);
  EditionReference::Push<BlockType::IntegerShort>(4);
  return multiplication;
}

QUIZ_CASE(pcj_create) {
  createSimpleExpression();
  CachePool::sharedCachePool()->editionPool()->flush();
}
