#include "expiring_pointer.h"
#include "storage_expression_model.h"
#include "storage_cartesian_function.h"
#include "storage_function.h"

namespace Shared {

#if DEBUG
template<>
StorageExpressionModel * ExpiringPointer<StorageExpressionModel>::s_global = nullptr;
template<>
StorageFunction * ExpiringPointer<StorageFunction>::s_global = nullptr;
template<>
StorageCartesianFunction * ExpiringPointer<StorageCartesianFunction>::s_global = nullptr;
#endif

}
