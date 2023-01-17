#include "dots.h"
#include <kandinsky/dot.h>

namespace Shared {

namespace Dots {

constexpr static const KDMask<TinyDotDiameter> tinyDot = KDMakeDot<TinyDotDiameter>(TinyDotRadius);
constexpr const uint8_t * TinyDotMask = &tinyDot.m_mask[0][0];

constexpr static const KDMask<SmallDotDiameter> smallDot = KDMakeDot<SmallDotDiameter>(SmallDotRadius);
constexpr const uint8_t * SmallDotMask = &smallDot.m_mask[0][0];

constexpr static const KDMask<MediumDotDiameter> mediumDot = KDMakeDot<MediumDotDiameter>(MediumDotRadius);
constexpr const uint8_t * MediumDotMask = &mediumDot.m_mask[0][0];

constexpr static const KDMask<LargeDotDiameter> largeDot = KDMakeDot<LargeDotDiameter>(LargeDotRadius);
constexpr const uint8_t * LargeDotMask = &largeDot.m_mask[0][0];

constexpr static const KDMask<LargeRingDiameter> largeRing = KDMakeRing<LargeRingDiameter>(LargeRingInternalRadius, LargeRingExternalRadius);
constexpr const uint8_t * LargeRingMask = &largeRing.m_mask[0][0];

}
}
