#ifndef SHARED_STORAGE_FUNCTION_STORE_H
#define SHARED_STORAGE_FUNCTION_STORE_H

#include "storage_function.h"
#include "storage_expression_model_store.h"
#include <stdint.h>

namespace Shared {

/* FunctionStore storse functions and gives them a color. The template should be
 * a class derived from Shared::Function */

template <class T>
class StorageFunctionStore : public StorageExpressionModelStore<T> {
public:
  StorageFunctionStore() : StorageExpressionModelStore<T>() {}
  uint32_t storeChecksum() {
    return Ion::Storage::sharedStorage()->checksum();
  }
  int numberOfActiveFunctions(); // An active function must be defined to be counted
  virtual T activeFunctionAtIndex(int i);
  T definedFunctionAtIndex(int i) { return static_cast<T>(StorageExpressionModelStore<T>::definedModelAtIndex(i)); } //TODO keep it?

  virtual char symbol() const = 0;
protected:
  static const char * const name(T f) { return f.name(); }
  static KDColor const color(T f) { return f.color(); }
  template<typename U> using AttributeGetter = U (*)(T f);
  template<typename U> U firstAvailableAttribute(U attributes[], AttributeGetter<U> attribute);
  const KDColor firstAvailableColor() {
    return firstAvailableAttribute(Palette::DataColor, StorageFunctionStore::color);
  }
private:
  virtual const char * firstAvailableName() = 0;
};

}

#endif
