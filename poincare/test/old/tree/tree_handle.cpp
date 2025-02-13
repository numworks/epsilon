#include <poincare/include/poincare/expression.h>
#include <poincare/init.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/pool_handle.h>
#include <quiz.h>

#include "blob_node.h"
#include "helpers.h"
#include "pair_node.h"

using namespace Poincare;

QUIZ_CASE(poincare_pool_handle_are_discared_after_block) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t = BlobByReference::Builder(0);
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

static void make_temp_blob() { PoolHandle t = BlobByReference::Builder(5); }
QUIZ_CASE(poincare_pool_handle_are_discared_after_function_call) {
  int initialPoolSize = pool_size();
  make_temp_blob();
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(poincare_pool_handle_can_be_copied) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t1 = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize + 1);
    PoolHandle t2 = t1;
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(poincare_pool_handle_can_be_moved) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize + 1);
    t = BlobByReference::Builder(456);
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(poincare_pool_object_release) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t1 = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize + 1);
    PoolHandle t2 = t1;
    assert_pool_size(initialPoolSize + 1);
    t1 = BlobByReference::Builder(456);
    assert_pool_size(initialPoolSize + 2);
    t2 = t1;
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

static PoolHandle blob_with_data_3() { return BlobByReference::Builder(3); }

QUIZ_CASE(poincare_pool_handle_can_be_returned) {
  int initialPoolSize = pool_size();
  PoolHandle t = blob_with_data_3();
  assert_pool_size(initialPoolSize + 1);
}

QUIZ_CASE(poincare_pool_handle_memory_failure) {
  int initialPoolSize = pool_size();
  int memoryFailureHasBeenHandled = false;
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    PoolHandle tree = BlobByReference::Builder(1);
    while (true) {
      tree = PairByReference::Builder(tree, BlobByReference::Builder(1));
    }
  } else {
    memoryFailureHasBeenHandled = true;
  }
  quiz_assert(memoryFailureHasBeenHandled);
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(poincare_pool_handle_does_not_copy) {
  int initialPoolSize = pool_size();
  PoolHandle t1 = BlobByReference::Builder(1);
  PoolHandle t2 = BlobByReference::Builder(2);
  assert_pool_size(initialPoolSize + 2);
  PoolHandle p1 = PairByReference::Builder(t1, t2);
  assert_pool_size(initialPoolSize + 3);
  PoolHandle p2 = p1;
  assert_pool_size(initialPoolSize + 3);
}

QUIZ_CASE(poincare_pool_expression_can_start_uninitialized) {
  int initialPoolSize = pool_size();
  Expression e;
  assert_pool_size(initialPoolSize);
  {
    Expression i = Expression::Builder(1);
    assert_pool_size(initialPoolSize + 1);
    e = i;
    assert_pool_size(initialPoolSize + 1);
  }
}

QUIZ_CASE(poincare_pool_expression_can_be_copied_even_if_uninitialized) {
  int initialPoolSize = pool_size();
  Expression e;
  Expression f;
  f = e;
  assert_pool_size(initialPoolSize);
}
