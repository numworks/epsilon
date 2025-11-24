#pragma once

#include <apps/i18n.h>
#include <assert.h>
#include <omg/string.h>
#include <poincare/print.h>

namespace Inference {

constexpr static size_t k_groupTitleBufferSize = 20;

// Returns a null-terminated string
OMG::String<k_groupTitleBufferSize> GroupTitle(int groupIndex,
                                               bool letterIndex = false);

}  // namespace Inference
