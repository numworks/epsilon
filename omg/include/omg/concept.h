#ifndef OMG_CONCEPT_H
#define OMG_CONCEPT_H

namespace OMG::Concept {

// less strict than derived_from from <concepts> that imposes is_convertible too
template <typename Derived, typename Base>
concept is_derived_from = __is_base_of(Base, Derived);

}  // namespace OMG::Concept

#endif
