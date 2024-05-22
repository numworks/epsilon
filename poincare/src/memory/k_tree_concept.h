#ifndef POINCARE_MEMORY_K_TREE_CONCEPT_H
#define POINCARE_MEMORY_K_TREE_CONCEPT_H

#include <omg/concept.h>

namespace Poincare::Internal::KTrees {
class AbstractKTree {};

/* Concept that gathers all KTrees */

template <class C>
concept KTreeConcept = OMG::Concept::is_derived_from<C, AbstractKTree>;
}  // namespace Poincare::Internal::KTrees

#endif
