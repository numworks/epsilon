#include "helper.h"

using namespace Poincare;

static inline void assert_equal(const Integer i, const Integer j)
{
  quiz_assert(Integer::NaturalOrder(i, j) == 0);
}

QUIZ_CASE(poincare_logic_xor_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(1), Integer::LogicOperation::Xor), Integer(14));
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(0), Integer::LogicOperation::Xor), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(15), Integer::LogicOperation::Xor), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, Integer(0), Integer::LogicOperation::Xor), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), hFFFF_FFFF, Integer::LogicOperation::Xor), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(0), Integer::LogicOperation::Xor), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(33)), h1_0000_0000);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(15)), Integer(32767));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(8)), Integer(255));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(4)), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(1)), Integer(1));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(0)), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer::LogicOperation::Xor, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::LogicalAndOrXor(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer::LogicOperation::Xor, Integer(65)), hFFFF_0000_0000_FFFF);
}

QUIZ_CASE(poincare_logic_or_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(1), Integer::LogicOperation::Or), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(0), Integer::LogicOperation::Or), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(15), Integer::LogicOperation::Or), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(15), Integer::LogicOperation::Or), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, Integer(0), Integer::LogicOperation::Or), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), hFFFF_FFFF, Integer::LogicOperation::Or), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, hFFFF_FFFF, Integer::LogicOperation::Or), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(0), Integer::LogicOperation::Or), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_0000_0000, Integer::LogicOperation::Or), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_0000_0000, Integer::LogicOperation::Or, Integer(33)), h1_0000_0000);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(15)), Integer(32767));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(8)), Integer(255));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(4)), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(1)), Integer(1));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::Or, Integer(0)), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer::LogicOperation::Or, Integer(64)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(h0000_FFFF_FFFF_0000, hFFFF_0000_0000_FFFF, Integer::LogicOperation::Or, Integer(65)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(h0000_FFFF_FFFF_0000, Integer(0), Integer::LogicOperation::Or, Integer(65)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_and_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(1), Integer::LogicOperation::And), Integer(1));
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(0), Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(15), Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(Integer(15), Integer(15), Integer::LogicOperation::And), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, Integer(0), Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), hFFFF_FFFF, Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, hFFFF_FFFF, Integer::LogicOperation::And), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_FFFF_FFFF, Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), Integer(0), Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_0000_0000, Integer::LogicOperation::And), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(h1_0000_0000, h1_0000_0000, Integer::LogicOperation::And), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::LogicalAndOrXor(Integer(0), h1_0000_0000, Integer::LogicOperation::And, Integer(33)), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(h1_0000_0000, h1_0000_0000, Integer::LogicOperation::And, Integer(33)), h1_0000_0000);
  assert_equal(Integer::LogicalAndOrXor(h1_0000_0000, h1_0000_0000, Integer::LogicOperation::And, Integer(34)), h1_0000_0000);
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(15)), Integer(32767));
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(8)), Integer(255));
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(4)), Integer(15));
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(1)), Integer(1));
  assert_equal(Integer::LogicalAndOrXor(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer::LogicOperation::And, Integer(0)), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer::LogicOperation::And, Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalAndOrXor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer::LogicOperation::And, Integer(90)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalAndOrXor(h0000_FFFF_FFFF_0000, hFFFF_0000_0000_FFFF, Integer::LogicOperation::And, Integer(65)), Integer(0));
  assert_equal(Integer::LogicalAndOrXor(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer::LogicOperation::And, Integer(65)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_not_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalNot(hFFFF_FFFF), Integer(0));
  assert_equal(Integer::LogicalNot(Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalNot(h1_FFFF_FFFF), Integer(0));
  assert_equal(Integer::LogicalNot(h1_0000_0000), hFFFF_FFFF);
  //explicit num_bits
  assert_equal(Integer::LogicalNot(hFFFF_FFFF, Integer(33)), h1_0000_0000);
  assert_equal(Integer::LogicalNot(Integer(0), Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(15)), Integer(32767));
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(8)), Integer(255));
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(4)), Integer(15));
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(1)), Integer(1));
  assert_equal(Integer::LogicalNot(h1_0000_0000, Integer(0)), Integer(0));
  assert_equal(Integer::LogicalNot(h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::LogicalNot(hFFFF_0000_0000_FFFF, Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalNot(hFFFF_FFFF_FFFF_FFFF, Integer(64)), Integer(0));
  assert_equal(Integer::LogicalNot(hFFFF_FFFF_FFFF_FFFF, Integer(51)), Integer(0));
  assert_equal(Integer::LogicalNot(Integer(0), Integer(64)), hFFFF_FFFF_FFFF_FFFF);
}

QUIZ_CASE(poincare_logic_sll_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer h1_0000_0000_0000_0000 = Integer("18446744073709551616");
  assert_equal(Integer::LogicalShift(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShift(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalShift(Integer(0), Integer(36)), Integer(0));
  assert_equal(Integer::LogicalShift(h1_0000_0000, Integer(32)), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalShift(Integer(1), Integer(32), Integer(33)), h1_0000_0000);
  assert_equal(Integer::LogicalShift(h0000_FFFF_FFFF_0000, Integer(16), Integer(64)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalShift(h0000_FFFF_FFFF_0000, Integer(16), Integer(65)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalShift(Integer(1), Integer(64), Integer(65)), h1_0000_0000_0000_0000);
}

QUIZ_CASE(poincare_logic_srl_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  assert_equal(Integer::LogicalShift(hFFFF_FFFF, Integer(-0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShift(Integer(0), Integer(-6)), Integer(0));
  assert_equal(Integer::LogicalShift(Integer(0), Integer(-36)), Integer(0));
  assert_equal(Integer::LogicalShift(h1_0000_0000, Integer(-32)), Integer(0));
  assert_equal(Integer::LogicalShift(hFFFF_FFFF, Integer(-32)), Integer(0));
  assert_equal(Integer::LogicalShift(hFFFF_FFFF, Integer(-31)), Integer(1));
  assert_equal(Integer::LogicalShift(hFFFF_FFFF, Integer(-30)), Integer(3));
  //explicit num_bits
  assert_equal(Integer::LogicalShift(h0000_FFFF_FFFF_0000, Integer(-32), Integer(33)), Integer(1));
  assert_equal(Integer::LogicalShift(h0000_FFFF_FFFF_0000, Integer(-32), Integer(64)), hFFFF);
  assert_equal(Integer::LogicalShift(h0000_FFFF_FFFF_0000, Integer(-32), Integer(66)), hFFFF);
}

QUIZ_CASE(poincare_logic_sra_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalShiftRightArithmetic(Integer(0), Integer(36)), Integer(0));
  assert_equal(Integer::LogicalShiftRightArithmetic(h1_0000_0000, Integer(32)), Integer(0));
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF_FFFF, Integer(32)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF_FFFF, Integer(3)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF_FFFF, Integer(30)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF, Integer(14)), Integer(3));
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF, Integer(15)), Integer(1));
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF, Integer(16)), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalShiftRightArithmetic(h0000_FFFF_FFFF_0000, Integer(16), Integer(64)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(h0000_FFFF_FFFF_0000, Integer(16), Integer(50)), hFFFF_FFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(h0000_FFFF_FFFF_0000, Integer(32), Integer(64)), hFFFF);
  assert_equal(Integer::LogicalShiftRightArithmetic(h0000_FFFF_FFFF_0000, Integer(32), Integer(4)), Integer(0));
  assert_equal(Integer::LogicalShiftRightArithmetic(hFFFF_FFFF_0000_0000, Integer(32), Integer(64)), hFFFF_FFFF_FFFF_FFFF);
}

QUIZ_CASE(poincare_logic_bit_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  for (uint8_t i = 0; i < 32; i++)
  {
    assert_equal(Integer::LogicalBitGet(hFFFF_FFFF, Integer(i)), Integer(1));
    assert_equal(Integer::LogicalBitGet(h1_0000_0000, Integer(i)), Integer(0));
  }
  assert_equal(Integer::LogicalBitGet(h1_0000_0000, Integer(32)), Integer(1));

  assert_equal(Integer::LogicalBitGet(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalBitGet(h0000_FFFF_FFFF_0000, Integer(32)), Integer(1));
  assert_equal(Integer::LogicalBitGet(h0000_FFFF_FFFF_0000, Integer(33)), Integer(1));
  assert_equal(Integer::LogicalBitGet(h0000_FFFF_FFFF_0000, Integer(47)), Integer(1));
  assert_equal(Integer::LogicalBitGet(h0000_FFFF_FFFF_0000, Integer(48)), Integer(0));
}

QUIZ_CASE(poincare_logic_ror_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(1)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(2)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(3)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(16)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(30)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF, Integer(31)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalRotateRight(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalRotateRight(h1_0000_0000, Integer(31)), Integer(0));
  assert_equal(Integer::LogicalRotateRight(h1_0000_0000, Integer(3)), Integer(0));
  assert_equal(Integer::LogicalRotateRight(hFFFF, Integer(16)), hFFFF_0000);
  assert_equal(Integer::LogicalRotateRight(Integer(2), Integer(1)), Integer(1));
  assert_equal(Integer::LogicalRotateRight(Integer(6), Integer(1)), Integer(3));
  //explicit num_bits
  assert_equal(Integer::LogicalRotateRight(h0000_FFFF_FFFF_0000, Integer(16), Integer(64)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(h0000_FFFF_FFFF_0000, Integer(16), Integer(50)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateRight(h0000_FFFF_FFFF_0000, Integer(32), Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::LogicalRotateRight(hFFFF_FFFF_0000_0000, Integer(16), Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalRotateRight(h0000_FFFF_FFFF_0000, Integer(48), Integer(64)), hFFFF_FFFF_0000_0000);
}

QUIZ_CASE(poincare_logic_rol_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(1)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(2)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(3)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(16)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(30)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(31)), hFFFF_FFFF);
  assert_equal(Integer::LogicalRotateLeft(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalRotateLeft(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalRotateLeft(h1_0000_0000, Integer(31)), Integer(0));
  assert_equal(Integer::LogicalRotateLeft(h1_0000_0000, Integer(3)), Integer(0));
  assert_equal(Integer::LogicalRotateLeft(hFFFF, Integer(16)), hFFFF_0000);
  assert_equal(Integer::LogicalRotateLeft(Integer(2), Integer(1)), Integer(4));
  assert_equal(Integer::LogicalRotateLeft(Integer(6), Integer(1)), Integer(12));
  //explicit num_bits
  assert_equal(Integer::LogicalRotateLeft(h0000_FFFF_FFFF_0000, Integer(16), Integer(64)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF, Integer(16), Integer(50)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalRotateLeft(h0000_FFFF_FFFF_0000, Integer(32), Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::LogicalRotateLeft(hFFFF_FFFF_0000_0000, Integer(16), Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::LogicalRotateLeft(h0000_FFFF_FFFF_0000, Integer(48), Integer(64)), hFFFF_FFFF);
}

QUIZ_CASE(poincare_logic_bic_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF, hFFFF), hFFFF_0000);
  assert_equal(Integer::LogicalBitsClear(hFFFF_0000, hFFFF), hFFFF_0000);
  assert_equal(Integer::LogicalBitsClear(Integer(7), Integer(2)), Integer(5));
  assert_equal(Integer::LogicalBitsClear(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalBitsClear(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalBitsClear(h1_0000_0000, Integer(31)), Integer(0));
  assert_equal(Integer::LogicalBitsClear(h1_0000_0000, Integer(3)), Integer(0));
  //explicit num_bits
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF_0000_0000, hFFFF_FFFF, Integer(64)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF_FFFF_FFFF, hFFFF_FFFF_0000_0000, Integer(64)), hFFFF_FFFF);
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF_FFFF_FFFF, hFFFF_0000_0000_FFFF, Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::LogicalBitsClear(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_0000_0000_FFFF);
}

QUIZ_CASE(poincare_logic_bclr_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h7FFF_FFFF_FFFF_FFFF = Integer("9223372036854775807");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalBitClear(Integer(1), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalBitClear(Integer(7), Integer(2)), Integer(3));
  assert_equal(Integer::LogicalBitClear(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalBitClear(Integer(0), Integer(6)), Integer(0));
  assert_equal(Integer::LogicalBitClear(h1_0000_0000, Integer(32)), Integer(0));
  assert_equal(Integer::LogicalBitClear(h1_0000_0000, Integer(3)), h1_0000_0000);
  assert_equal(Integer::LogicalBitClear(hFFFF_FFFF_0000_0000, Integer(64)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalBitClear(hFFFF_FFFF_FFFF_FFFF, Integer(64)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::LogicalBitClear(hFFFF_FFFF_FFFF_FFFF, Integer(63)), h7FFF_FFFF_FFFF_FFFF);
}

QUIZ_CASE(poincare_logic_bset_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h7FFF_FFFF_FFFF_FFFF = Integer("9223372036854775807");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalBitSet(Integer(1), Integer(0)), Integer(1));
  assert_equal(Integer::LogicalBitSet(Integer(0), Integer(0)), Integer(1));
  assert_equal(Integer::LogicalBitSet(Integer(3), Integer(2)), Integer(7));
  assert_equal(Integer::LogicalBitSet(Integer(0), Integer(8)), Integer(256));
  assert_equal(Integer::LogicalBitSet(Integer(0), Integer(32)), h1_0000_0000);
  assert_equal(Integer::LogicalBitSet(hFFFF_FFFF_0000_0000, Integer(63)), hFFFF_FFFF_0000_0000);
  assert_equal(Integer::LogicalBitSet(hFFFF_FFFF_FFFF_FFFF, Integer(60)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::LogicalBitSet(h7FFF_FFFF_FFFF_FFFF, Integer(63)), hFFFF_FFFF_FFFF_FFFF);
}

QUIZ_CASE(poincare_logic_bflip_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h7FFF_FFFF_FFFF_FFFF = Integer("9223372036854775807");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::LogicalBitFlip(Integer(1), Integer(0)), Integer(0));
  assert_equal(Integer::LogicalBitFlip(Integer(0), Integer(0)), Integer(1));
  assert_equal(Integer::LogicalBitFlip(Integer(3), Integer(2)), Integer(7));
  assert_equal(Integer::LogicalBitFlip(Integer(7), Integer(2)), Integer(3));
  assert_equal(Integer::LogicalBitFlip(Integer(1), Integer(8)), Integer(257));
  assert_equal(Integer::LogicalBitFlip(Integer(257), Integer(8)), Integer(1));
  assert_equal(Integer::LogicalBitFlip(Integer(0), Integer(32)), h1_0000_0000);
  assert_equal(Integer::LogicalBitFlip(h1_0000_0000, Integer(32)), Integer(0));
  assert_equal(Integer::LogicalBitFlip(h7FFF_FFFF_FFFF_FFFF, Integer(63)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::LogicalBitFlip(hFFFF_FFFF_FFFF_FFFF, Integer(63)), h7FFF_FFFF_FFFF_FFFF);
}

QUIZ_CASE(poincare_logic_tc_compare)
{
  Integer hFFFF = Integer("65535");
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer hFFFF_0000 = Integer("4294901760");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_FFFF_0000_0000 = Integer("18446744069414584320");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h7FFF_FFFF_FFFF_FFFF = Integer("9223372036854775807");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::TwosComplementToBits(Integer("-1"), Integer(64)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::TwosComplementToBits(Integer("-1"), Integer(63)), h7FFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::TwosComplementToBits(Integer("-1"), Integer(32)), hFFFF_FFFF);
  assert_equal(Integer::TwosComplementToBits(Integer("-1"), Integer(16)), hFFFF);
  assert_equal(Integer::TwosComplementToBits(Integer("-4294967296"), Integer(33)), h1_0000_0000);

  assert_equal(Integer::TwosComplementToBits(hFFFF_FFFF, Integer(32)), Integer("-1"));
  assert_equal(Integer::TwosComplementToBits(hFFFF, Integer(17)), Integer("65535"));
  assert_equal(Integer::TwosComplementToBits(hFFFF, Integer(16)), Integer("-1"));
  assert_equal(Integer::TwosComplementToBits(h7FFF_FFFF_FFFF_FFFF, Integer(63)), Integer("-1"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_FFFF_FFFF_FFFF, Integer(63)), Integer("-1"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_FFFF_FFFF_FFFF, Integer(64)), Integer("-1"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_FFFF_0000_0000, Integer(64)), Integer("-4294967296"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_0000_0000_FFFF, Integer(64)), Integer("-281474976645121"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_0000_0000_FFFF, Integer(63)), Integer("-281474976645121"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_0000_0000_FFFF, Integer(59)), Integer("-281474976645121"));
  assert_equal(Integer::TwosComplementToBits(h1_0000_0000, Integer(33)), Integer("-4294967296"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_0000, Integer(33)), Integer("4294901760"));
  assert_equal(Integer::TwosComplementToBits(hFFFF_0000, Integer(17)), Integer("-65536"));
}