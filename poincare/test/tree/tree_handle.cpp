#include <quiz.h>
#include <poincare/tree_handle.h>
#include <poincare/init.h>
#include <poincare/exception_checkpoint.h>
#include <assert.h>
#include "blob_node.h"
#include "pair_node.h"

#include "helpers.h"

using namespace Poincare;

QUIZ_CASE(tree_handle_are_discared_after_block) {
  int initialPoolSize = pool_size();
  {
    BlobByReference b(0);
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

static void make_temp_blob() {
  BlobByReference b(5);
}
QUIZ_CASE(tree_handle_are_discared_after_function_call) {
  int initialPoolSize = pool_size();
  make_temp_blob();
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_handle_can_be_copied) {
  int initialPoolSize = pool_size();
  {
    BlobByReference b(123);
    assert_pool_size(initialPoolSize+1);
    TreeHandle t = b;
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_handle_can_be_moved) {
  int initialPoolSize = pool_size();
  {
    TreeHandle t = BlobByReference(123);
    assert_pool_size(initialPoolSize+1);
  }
  {
    TreeHandle t = BlobByReference(123);
    t = BlobByReference(456);
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

static TreeHandle blob_with_data_3() {
  return BlobByReference(3);
}

QUIZ_CASE(tree_handle_can_be_returned) {
  int initialPoolSize = pool_size();
  TreeHandle b = blob_with_data_3();
  assert_pool_size(initialPoolSize+1);
}

QUIZ_CASE(tree_handle_memory_failure) {
  int initialPoolSize = pool_size();
  int memoryFailureHasBeenHandled = false;
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    TreeHandle tree = BlobByReference(1);
    while (true) {
      tree = PairByReference(tree, BlobByReference(1));
    }
  } else {
    Poincare::Tidy();
    memoryFailureHasBeenHandled = true;
  }
  quiz_assert(memoryFailureHasBeenHandled);
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_handle_does_not_copy) {
  int initialPoolSize = pool_size();
  BlobByReference b1(1);
  BlobByReference b2(2);
  assert_pool_size(initialPoolSize+2);
  PairByReference p(b1, b2);
  assert_pool_size(initialPoolSize+3);
  PairByReference p2 = p;
  assert_pool_size(initialPoolSize+3);
}
