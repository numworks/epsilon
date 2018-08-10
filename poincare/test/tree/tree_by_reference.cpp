#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#include "helpers.h"

using namespace Poincare;

QUIZ_CASE(tree_by_reference_are_discared_after_block) {
  assert_pool_size(0);
  {
    BlobByReference b(0);
    assert_pool_size(1);
  }
  assert_pool_size(0);
}

static void make_temp_blob() {
  BlobByReference b(5);
}
QUIZ_CASE(tree_by_reference_are_discared_after_function_call) {
  assert_pool_size(0);
  make_temp_blob();
  assert_pool_size(0);
}

QUIZ_CASE(tree_by_reference_can_be_copied) {
  assert_pool_size(0);
  BlobByReference b(123);
  assert_pool_size(1);
  TreeByReference t = b;
  assert_pool_size(1);
}

static TreeByReference blob_with_data_3() {
  return BlobByReference(3);
}
QUIZ_CASE(tree_by_reference_can_be_returned) {
  assert_pool_size(0);
  TreeByReference b = blob_with_data_3();
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_reference_allocation_failures) {
  assert_pool_size(0);
  BlobByReference b(1);
  assert_pool_size(1);
  {
    BlobByReference array[100];
    assert(pool_size() > 1);
    assert(pool_size() < 100);
  }
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_reference_does_not_copy) {
  assert_pool_size(0);
  BlobByReference b1(1);
  BlobByReference b2(2);
  assert_pool_size(2);
  PairByReference p(b1, b2);
  assert_pool_size(3);
  PairByReference p2 = p;
  assert_pool_size(3);
}
