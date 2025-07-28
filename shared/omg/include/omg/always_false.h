#ifndef OMG_ALWAYS_FALSE_H
#define OMG_ALWAYS_FALSE_H

namespace OMG {

/* A false boolean with a useless dependency on some templates parameters
 *
 * To be used in static_assert when some template should give an error when
 * instanciated. A simple static_assert(false) would fail too early.
 */

template <class... T>
constexpr bool always_false = false;

}  // namespace OMG

#endif
