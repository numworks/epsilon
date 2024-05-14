#ifndef SHARED_FUNCTION_MODELS_NAMES_H
#define SHARED_FUNCTION_MODELS_NAMES_H

#include <apps/shared/continuous_function.h>
#include <omg/code_point.h>

#include <array>

namespace Shared {

namespace FunctionNameHelper {

size_t AddSuffixForParametricComponent(char* baseName, size_t baseNameLength,
                                       size_t bufferSize, bool first);
size_t ParametricComponentNameWithArgument(Shared::ContinuousFunction* f,
                                           char* buffer, size_t bufferSize,
                                           bool first, int derivationOrder = 0);
bool ParametricComponentsNamesAreFree(char* baseName, size_t baseNameLength,
                                      size_t bufferSize);
int DefaultName(char* buffer, size_t bufferSize, CodePoint symbol);
bool ParametricComponentsNameError(Poincare::UserExpression expression,
                                   Shared::ContinuousFunction* function);

}  // namespace FunctionNameHelper

}  // namespace Shared

#endif
