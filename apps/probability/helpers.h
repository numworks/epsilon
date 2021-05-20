#ifndef HELPER_H
#define HELPER_H

#include <apps/i18n.h>
#include <apps/shared/text_field_delegate_app.h>
#include <assert.h>
#include <escher/container.h>

#include "data.h"

namespace Probability {

Shared::TextFieldDelegateApp * getProbabilityApp();

int testToText(Data::Test t, char * buffer, int bufferLength);
int testTypeToText(Data::TestType t, char * buffer, int bufferLength);

}  // namespace Probability

#endif /* HELPER_H */
