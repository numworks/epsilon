#include <poincare/sign.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

#if POINCARE_TREE_LOG
void Sign::log(std::ostream& stream, bool endOfLine) const {
  if (isNull()) {
    stream << "Zero";
  } else {
    if (isUnknown()) {
      /* m_canBeStrictlyNegative=1 and m_canBeStrictlyPositive=1 and
       * m_canBeNull=1 */
      stream << "Unknown";
    } else {
      if (m_canBeStrictlyPositive && m_canBeStrictlyNegative) {
        /* The sign is not unknown so if m_canBeStrictlyPositive=1 and
         * m_canBeStrictlyNegative=1, then m_canBeNull=0. But it also means that
         * we don't know whether the sign is positive or negative. */
        stream << "Non Null";
      } else {
        /* Either m_canBeStrictlyNegative=1 and then m_canBeStrictlyPositive=0,
         * or m_canBeStrictlyNegative=0 and then m_canBeStrictlyPositive=1 */
        if (!m_canBeNull) {
          stream << "Strictly ";
        }
        stream << (m_canBeStrictlyNegative ? "Negative" : "Positive");
      }
    }
  }
  if (endOfLine) {
    stream << "\n";
  }
}
#endif

namespace Internal {

Sign GetSign(const Tree* e) { return GetProperties(e).sign(); }

}  // namespace Internal

}  // namespace Poincare
