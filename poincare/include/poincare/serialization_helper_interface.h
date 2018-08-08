#ifndef POINCARE_SERIALIZATION_HELPER_INTERFACE_H
#define POINCARE_SERIALIZATION_HELPER_INTERFACE_H

#include <poincare/preferences.h>

namespace Poincare {

class SerializationHelperInterface {
public:
  virtual int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const = 0;
  virtual bool needsParenthesesWithParent(const SerializationHelperInterface * parent) const { return false; };

  virtual SerializationHelperInterface * serializableChildAtIndex(int i) const = 0;
  virtual int numberOfSerializableChildren() const = 0;
};

}

#endif
