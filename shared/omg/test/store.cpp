#include <omg/store.h>
#include <quiz.h>
#include <string.h>

using namespace OMG;

QUIZ_CASE(omg_store) {
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  Store store(buffer, bufferSize);
  quiz_assert(store.maximumSize() == 18);
  store.pushElement("strtofillthestore", 18);
  quiz_assert(store.remainingSize() == 0);
  store.deleteElementAtIndex(0);
  quiz_assert(store.remainingSize() == store.maximumSize());

  store.pushElement("hello", 6);
  quiz_assert(store.remainingSize() == 10);
  store.pushElement("world", 6);
  quiz_assert(store.remainingSize() == 2);
  store.pushElement("!", 2);
  quiz_assert(store.remainingSize() == 0);

  store.updateElementAtIndex(0, "hi", 3);
  // Since remainingSize deducts space for a new offset, updateElement changes
  // the remainingSize in a surprising way.
  quiz_assert(store.remainingSize() == 1);
  store.updateElementAtIndex(1, "everyone", 9);
  quiz_assert(store.remainingSize() == 0);

  store.makeRoomForElement(13);
  quiz_assert(store.numberOfElements() == 1);
  store.pushElement("a larger one", 13);

  quiz_assert(strcmp((char*)store.elementAtIndex(0), "!") == 0);
  quiz_assert(strcmp((char*)store.elementAtIndex(1), "a larger one") == 0);
}
