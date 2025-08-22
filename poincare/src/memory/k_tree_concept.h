#pragma once

#include <omg/concept.h>

#include <type_traits>

namespace Poincare::Internal::KTrees {

class AbstractKTree {};

/* Concept that gathers all KTrees */

template <class C>
concept KTreeConcept = OMG::Concept::is_derived_from<C, AbstractKTree>;

template <typename TSC, class... Args>
concept HasATreeConcept = (false || ... ||
                           std::is_same<const TSC*, Args>::value);

}  // namespace Poincare::Internal::KTrees
