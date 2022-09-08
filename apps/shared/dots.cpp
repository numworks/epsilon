#include "dots.h"
#include <kandinsky/dot.h>

namespace Shared {

constexpr static const KDMask<Dots::TinyDotDiameter> tinyDot = KDMakeDot<Dots::TinyDotDiameter>(Dots::TinyDotRadius);
const uint8_t (&Dots::TinyDotMask)[Dots::TinyDotDiameter][Dots::TinyDotDiameter] = tinyDot.m_mask;

constexpr static const KDMask<Dots::SmallDotDiameter> smallDot = KDMakeDot<Dots::SmallDotDiameter>(Dots::SmallDotRadius);
const uint8_t (&Dots::SmallDotMask)[Dots::SmallDotDiameter][Dots::SmallDotDiameter] = smallDot.m_mask;

constexpr static const KDMask<Dots::MediumDotDiameter> mediumDot = KDMakeDot<Dots::MediumDotDiameter>(Dots::MediumDotRadius);
const uint8_t (&Dots::MediumDotMask)[Dots::MediumDotDiameter][Dots::MediumDotDiameter] = mediumDot.m_mask;

constexpr static const KDMask<Dots::LargeDotDiameter> largeDot = KDMakeDot<Dots::LargeDotDiameter>(Dots::LargeDotRadius);
const uint8_t (&Dots::LargeDotMask)[Dots::LargeDotDiameter][Dots::LargeDotDiameter] = largeDot.m_mask;

constexpr static const KDMask<Dots::LargeRingDiameter> largeRing = KDMakeRing<Dots::LargeRingDiameter>(Dots::LargeRingInternalRadius, Dots::LargeRingExternalRadius);
const uint8_t (&Dots::LargeRingMask)[Dots::LargeRingDiameter][Dots::LargeRingDiameter] = largeRing.m_mask;

}
