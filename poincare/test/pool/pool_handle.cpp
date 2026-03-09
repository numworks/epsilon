#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/include/poincare/expression.h>
#include <poincare/init.h>
#include <poincare/pool_handle.h>
#include <poincare/system_expression.h>
#include <poincare/user_expression.h>
#include <quiz.h>

#include "blob_object.h"
#include "helpers.h"
#include "pair_object.h"

using namespace Poincare;

QUIZ_CASE(pcj_pool_handle_are_discared_after_block) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t = BlobByReference::Builder(0);
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

static void make_temp_blob() { PoolHandle t = BlobByReference::Builder(5); }
QUIZ_CASE(pcj_pool_handle_are_discared_after_function_call) {
  int initialPoolSize = pool_size();
  make_temp_blob();
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_handle_can_be_copied) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t1 = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize + 1);
    PoolHandle t2 = t1;
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_handle_can_be_moved) {
  int initialPoolSize = pool_size();
  {
    PoolHandle t = BlobByReference::Builder(123);
    assert_pool_size(initialPoolSize + 1);
    t = BlobByReference::Builder(456);
    assert_pool_size(initialPoolSize + 1);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_object_release) {
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

QUIZ_CASE(pcj_pool_handle_can_be_returned) {
  int initialPoolSize = pool_size();
  PoolHandle t = blob_with_data_3();
  assert_pool_size(initialPoolSize + 1);
}

QUIZ_CASE(pcj_pool_handle_memory_failure) {
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

QUIZ_CASE(pcj_pool_handle_does_not_copy) {
  int initialPoolSize = pool_size();
  PoolHandle t1 = BlobByReference::Builder(1);
  PoolHandle t2 = BlobByReference::Builder(2);
  assert_pool_size(initialPoolSize + 2);
  PoolHandle p1 = PairByReference::Builder(t1, t2);
  assert_pool_size(initialPoolSize + 3);
  PoolHandle p2 = p1;
  assert_pool_size(initialPoolSize + 3);
}

QUIZ_CASE(pcj_pool_expression_can_start_uninitialized) {
  int initialPoolSize = pool_size();
  Expression e;
  assert_pool_size(initialPoolSize);
  {
    UserExpression i = UserExpression::Builder(1);
    assert_pool_size(initialPoolSize + 1);
    e = i;
    assert_pool_size(initialPoolSize + 1);
  }
}

QUIZ_CASE(pcj_pool_expression_can_be_copied_even_if_uninitialized) {
  int initialPoolSize = pool_size();
  Expression e;
  Expression f;
  f = e;
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_handle_failure_with_retained_object_after_checkpoint) {
  int initialPoolSize = pool_size();
  bool exceptionIsHandled = false;
  {
    Expression e;
    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      // e retains
      e = UserExpression::Builder(1);
      assert_pool_size(initialPoolSize + 1);
      assert(e.objectRetainCount() == 1);
      ExceptionCheckpoint::Raise();
    } else {
      exceptionIsHandled = true;
    }
    // The pool was properly rolled back
    quiz_assert(exceptionIsHandled);
    assert_pool_size(initialPoolSize);
    assert(e.wasErasedByException());
  }
  // e destructor properly handled the absence of its object
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_handle_failure_with_retained_object_before_checkpoint_1) {
  int initialPoolSize = pool_size();
  bool exceptionIsHandled = false;
  {
    UserExpression e1 = UserExpression::Builder(1);
    assert_pool_size(initialPoolSize + 1);
    assert(e1.objectRetainCount() == 1);

    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      // TODO: e2 doesn't retain
      UserExpression e2 = UserExpression(e1);
      assert_pool_size(initialPoolSize + 1);
      assert(e1.objectRetainCount() == 1);
      ExceptionCheckpoint::Raise();
    } else {
      exceptionIsHandled = true;
    }
    quiz_assert(exceptionIsHandled);
    assert_pool_size(initialPoolSize + 1);
    assert(e1.objectRetainCount() == 1);
  }
  assert_pool_size(initialPoolSize);
}

// ---- TODO: All the following tests are failing ----

/**
QUIZ_CASE(pcj_pool_handle_failure_with_retained_object_before_checkpoint_2) {
  int initialPoolSize = pool_size();
  bool exceptionIsHandled = false;
  {
    Expression e1;
    UserExpression e2 = UserExpression::Builder(1);
    assert_pool_size(initialPoolSize + 1);
    {
      Poincare::ExceptionCheckpoint ecp;
      if (ExceptionRun(ecp)) {
        // TODO: e1 doesn't retain e2 because it's before the checkpoint
        e1 = e2;
        assert_pool_size(initialPoolSize + 1);
        assert(e1.objectRetainCount() == 1);
      } else {
        exceptionIsHandled = true;
      }
      quiz_assert(!exceptionIsHandled);
      assert_pool_size(initialPoolSize + 1);
      // e1 still points to e2
      assert(e1.objectRetainCount() == 1);
      assert(e2.objectRetainCount() == 1);
    }
    e2 = UserExpression::Builder(2);
    // ERROR: e2 releases and e1 is invalid
    assert(e1.objectRetainCount() == 1);
    assert_pool_size(initialPoolSize + 2);
  }
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(pcj_pool_handle_failure_with_released_object_before_checkpoint) {
  int initialPoolSize = pool_size();
  bool exceptionIsHandled = false;
  {
    UserExpression e1 = UserExpression::Builder(1);
    assert_pool_size(initialPoolSize + 1);
    assert(e1.objectRetainCount() == 1);

    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      // TODO: e2 doesn't retain e1 because it's before the checkpoint
      UserExpression e2 = UserExpression(e1);
      assert_pool_size(initialPoolSize + 1);
      assert(e1.objectRetainCount() == 1);

      // TODO: e1 isn't released!, because it's before the checkpoint
      // (but e2 is thus still valid)
      e1 = UserExpression::Builder(3);
      assert_pool_size(initialPoolSize + 2);
      assert(e1.objectRetainCount() == 1);
      assert(e2.objectRetainCount() == 1);
      ExceptionCheckpoint::Raise();
    } else {
      exceptionIsHandled = true;
    }
    quiz_assert(exceptionIsHandled);
    assert(e1.wasErasedByException());

    assert_pool_size(initialPoolSize + 1);
  }
  // ERROR: old value of e1 is not released
  assert_pool_size(initialPoolSize);
}
**/
