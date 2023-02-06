#include <quiz.h>
#include "../continuous_function.h"
#include "../continuous_function_store.h"
#include "../sequence.h"
#include "../sequence_store.h"

namespace Shared {

template<class F>
void interactWithBaseRecordMember(F * fct) {
  /* Accessing Function record member m_color, which has a 2-byte alignment
   * Only effective in DEBUG=1, as there are no compiler optimizations */
  KDColor color = fct->color();
  (void) color; // Silence compilation warning about unused variable.
}

void interactWithRecordMember(SequenceStore * store, Ion::Storage::Record rec) {
  Sequence * seq = store->modelForRecord(rec);
  /* Setting Sequence type will write record member m_initialConditionSizes,
   * which has a 2-byte alignment */
  seq->setType(Sequence::Type::SingleRecurrence);
  interactWithBaseRecordMember<Sequence>(seq);
}

void interactWithRecordMember(ContinuousFunctionStore * store, Ion::Storage::Record rec) {
  ContinuousFunction * fct = store->modelForRecord(rec).pointer();
  // Setting m_min from record member m_domain, which has a 2-byte alignment
  fct->setTAuto(false);
  fct->setTMin(3.0f);
  interactWithBaseRecordMember<ContinuousFunction>(fct);
}

template<class T>
Ion::Storage::Record createRecord(T * store) {
  Ion::Storage::Record::ErrorStatus err = store->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void) err; // Silence compilation warning about unused variable.
  return store->recordAtIndex(store->numberOfModels()-1);
}

template<class T>
void testAlignmentHandlingFor() {
  T store;
  Ion::Storage::FileSystem * sharedFileSystem = Ion::Storage::FileSystem::sharedFileSystem;

  sharedFileSystem->destroyAllRecords();
  Ion::Storage::Record rec1 = createRecord<T>(&store);
  // Evaluate the sequence shift compared to a 2-byte alignment
  uintptr_t shift = reinterpret_cast<uintptr_t>(rec1.value().buffer) % 2;

  /* Interact with an alignment sensitive member. A mishandled record alignment
   * should throw an abort(alignment fault) exception */
  interactWithRecordMember(&store, rec1);

  sharedFileSystem->destroyAllRecords();
  // Repeat the same process with a 1 byte record padding
  Ion::Storage::Record::ErrorStatus err = sharedFileSystem->createRecordWithExtension("1", "1", "1", 1);
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void) err; // Silence compilation warning about unused variable.

  Ion::Storage::Record rec2 = createRecord<T>(&store);
  shift += reinterpret_cast<uintptr_t>(rec2.value().buffer) % 2;
  interactWithRecordMember(&store, rec2);

  /* If fct1 and fct2 had different shifts, the test is able to detect a
   * mishandled record alignment */
  quiz_assert(shift == 1);

  sharedFileSystem->destroyAllRecords();
}

QUIZ_CASE(alignment_handling) {
  /* This test main function is to crash if storage alignment is not handled
   * properly. It also ensures that the right test - load and store of
   * differently-aligned objects - is performed (if storage/record
   * implementations change for instance). */
  testAlignmentHandlingFor<SequenceStore>();
  testAlignmentHandlingFor<ContinuousFunctionStore>();
}

}
