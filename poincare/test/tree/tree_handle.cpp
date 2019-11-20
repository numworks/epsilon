#include <quiz.h>
#include <poincare/tree_handle.h>
#include <poincare/init.h>
#include <poincare/exception_checkpoint.h>
#include "blob_node.h"
#include "pair_node.h"

#include "helpers.h"

using namespace Poincare;

QUIZ_CASE(tree_handle_are_discared_after_block) {
  int initialPoolSize = pool_size();
  {
    BlobByReference b = BlobByReference::Builder(0);
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

static void make_temp_blob() {
  BlobByReference b = BlobByReference::Builder(5);
}
QUIZ_CASE(tree_handle_are_discared_after_function_call) {
  int initialPoolSize = pool_size();
  make_temp_blob();
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_handle_can_be_copied) {
  int initialPoolSize = pool_size();
  {
    BlobByReference b = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize+1);
    TreeHandle t = b;
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_handle_can_be_moved) {
  int initialPoolSize = pool_size();
  {
    TreeHandle t = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize+1);
  }
  {
    TreeHandle t = BlobByReference::Builder(123);
    t = BlobByReference::Builder(456);
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

static TreeHandle blob_with_data_3() {
  return BlobByReference::Builder(3);
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
    TreeHandle tree = BlobByReference::Builder(1);
    while (true) {
      tree = PairByReference::Builder(tree, BlobByReference::Builder(1));
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
  BlobByReference b1 = BlobByReference::Builder(1);
  BlobByReference b2 = BlobByReference::Builder(2);
  assert_pool_size(initialPoolSize+2);
  PairByReference p = PairByReference::Builder(b1, b2);
  assert_pool_size(initialPoolSize+3);
  PairByReference p2 = p;
  assert_pool_size(initialPoolSize+3);
}
