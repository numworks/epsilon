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
  assert_equal(Integer::Xor(Integer(15), Integer(1)), Integer(14));
  assert_equal(Integer::Xor(Integer(15), Integer(0)), Integer(15));
  assert_equal(Integer::Xor(Integer(0), Integer(15)), Integer(15));
  assert_equal(Integer::Xor(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Xor(Integer(0), hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Xor(Integer(0), Integer(0)), Integer(0));
  //explicit num_bits
  assert_equal(Integer::Xor(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), h1_0000_0000);
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(15)), Integer(32767));
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(8)), Integer(255));
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(4)), Integer(15));
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(1)), Integer(1));
  assert_equal(Integer::Xor(Integer(0), h1_FFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::Xor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::Xor(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer(65)), hFFFF_0000_0000_FFFF);
}

QUIZ_CASE(poincare_logic_xnor_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::Xnor(hFFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::Xnor(Integer(0), hFFFF_FFFF), Integer(0));
  assert_equal(Integer::Xnor(hFFFF_FFFF, hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Xnor(Integer(0), h1_FFFF_FFFF), Integer(0));
  assert_equal(Integer::Xnor(Integer(0), Integer(0)), hFFFF_FFFF);
  //explicit num_bits
  assert_equal(Integer::Xnor(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::Xnor(Integer(0), h1_FFFF_FFFF, Integer(33)), Integer(0));
  assert_equal(Integer::Xnor(Integer(0), h1_0000_0000, Integer(15)), Integer(32767));
  assert_equal(Integer::Xnor(Integer(0), h1_0000_0000, Integer(8)), Integer(255));
  assert_equal(Integer::Xnor(Integer(0), h1_0000_0000, Integer(4)), Integer(15));
  assert_equal(Integer::Xnor(Integer(0), h1_0000_0000, Integer(1)), Integer(1));
  assert_equal(Integer::Xnor(Integer(0), h1_0000_0000, Integer(0)), Integer(0));
  assert_equal(Integer::Xnor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(63)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::Xnor(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer(64)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_or_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::Or(Integer(15), Integer(1)), Integer(15));
  assert_equal(Integer::Or(Integer(15), Integer(0)), Integer(15));
  assert_equal(Integer::Or(Integer(0), Integer(15)), Integer(15));
  assert_equal(Integer::Or(Integer(15), Integer(15)), Integer(15));
  assert_equal(Integer::Or(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Or(Integer(0), hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Or(hFFFF_FFFF, hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Or(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::Or(Integer(0), h1_0000_0000), Integer(0));
  //explicit num_bits
  assert_equal(Integer::Or(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::Or(Integer(0), h1_0000_0000, Integer(33)), h1_0000_0000);
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF, Integer(15)), Integer(32767));
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF, Integer(8)), Integer(255));
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF, Integer(4)), Integer(15));
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF, Integer(1)), Integer(1));
  assert_equal(Integer::Or(Integer(0), h1_FFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::Or(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::Or(h0000_FFFF_FFFF_0000, hFFFF_0000_0000_FFFF, Integer(65)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::Or(h0000_FFFF_FFFF_0000, Integer(0), Integer(65)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_nor_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::Nor(hFFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::Nor(Integer(0), hFFFF_FFFF), Integer(0));
  assert_equal(Integer::Nor(hFFFF_FFFF, hFFFF_FFFF), Integer(0));
  assert_equal(Integer::Nor(Integer(0), h1_FFFF_FFFF), Integer(0));
  assert_equal(Integer::Nor(Integer(0), Integer(0)), hFFFF_FFFF);
  //explicit num_bits
  assert_equal(Integer::Nor(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), Integer(0));
  assert_equal(Integer::Nor(Integer(0), h1_FFFF_FFFF, Integer(33)), Integer(0));
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(15)), Integer(32767));
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(8)), Integer(255));
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(4)), Integer(15));
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(1)), Integer(1));
  assert_equal(Integer::Nor(Integer(0), h1_0000_0000, Integer(0)), Integer(0));
  assert_equal(Integer::Nor(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(63)), Integer(0));
  assert_equal(Integer::Nor(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer(64)), Integer(0));
  assert_equal(Integer::Nor(h0000_FFFF_FFFF_0000, Integer(0), Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::Nor(hFFFF_0000_0000_FFFF, Integer(0), Integer(64)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_and_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::And(Integer(15), Integer(1)), Integer(1));
  assert_equal(Integer::And(Integer(15), Integer(0)), Integer(0));
  assert_equal(Integer::And(Integer(0), Integer(15)), Integer(0));
  assert_equal(Integer::And(Integer(15), Integer(15)), Integer(15));
  assert_equal(Integer::And(hFFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::And(Integer(0), hFFFF_FFFF), Integer(0));
  assert_equal(Integer::And(hFFFF_FFFF, hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::And(Integer(0), h1_FFFF_FFFF), Integer(0));
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::And(Integer(0), Integer(0)), Integer(0));
  assert_equal(Integer::And(Integer(0), h1_0000_0000), Integer(0));
  assert_equal(Integer::And(h1_0000_0000, h1_0000_0000), Integer(0));
  //explicit num_bits
  assert_equal(Integer::And(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::And(Integer(0), h1_0000_0000, Integer(33)), Integer(0));
  assert_equal(Integer::And(h1_0000_0000, h1_0000_0000, Integer(33)), h1_0000_0000);
  assert_equal(Integer::And(h1_0000_0000, h1_0000_0000, Integer(34)), h1_0000_0000);
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer(15)), Integer(32767));
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer(8)), Integer(255));
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer(4)), Integer(15));
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer(1)), Integer(1));
  assert_equal(Integer::And(h1_FFFF_FFFF, h1_FFFF_FFFF, Integer(0)), Integer(0));
  assert_equal(Integer::And(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::And(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(90)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::And(h0000_FFFF_FFFF_0000, hFFFF_0000_0000_FFFF, Integer(65)), Integer(0));
  assert_equal(Integer::And(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer(65)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_nand_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::Nand(hFFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Nand(Integer(0), hFFFF_FFFF), hFFFF_FFFF);
  assert_equal(Integer::Nand(hFFFF_FFFF, hFFFF_FFFF), Integer(0));
  assert_equal(Integer::Nand(Integer(0), Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Nand(h1_FFFF_FFFF, Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Nand(Integer(0), h1_0000_0000), hFFFF_FFFF);
  assert_equal(Integer::Nand(h1_0000_0000, h1_0000_0000), hFFFF_FFFF);
  //explicit num_bits
  assert_equal(Integer::Nand(hFFFF_FFFF, h1_FFFF_FFFF, Integer(33)), h1_0000_0000);
  assert_equal(Integer::Nand(Integer(0), h1_0000_0000, Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::Nand(h1_0000_0000, h1_0000_0000, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::Nand(h1_0000_0000, h1_0000_0000, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::Nand(h1_FFFF_FFFF, h1_0000_0000, Integer(15)), Integer(32767));
  assert_equal(Integer::Nand(h1_FFFF_FFFF, h1_0000_0000, Integer(8)), Integer(255));
  assert_equal(Integer::Nand(h1_FFFF_FFFF, h1_0000_0000, Integer(4)), Integer(15));
  assert_equal(Integer::Nand(h1_FFFF_FFFF, h1_0000_0000, Integer(1)), Integer(1));
  assert_equal(Integer::Nand(h1_FFFF_FFFF, h1_0000_0000, Integer(0)), Integer(0));
  assert_equal(Integer::Nand(hFFFF_FFFF_FFFF_FFFF, h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::Nand(h0000_FFFF_FFFF_0000, hFFFF_0000_0000_FFFF, Integer(64)), hFFFF_FFFF_FFFF_FFFF);
  assert_equal(Integer::Nand(h0000_FFFF_FFFF_0000, hFFFF_FFFF_FFFF_FFFF, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::Nand(hFFFF_0000_0000_FFFF, hFFFF_FFFF_FFFF_FFFF, Integer(64)), h0000_FFFF_FFFF_0000);
}

QUIZ_CASE(poincare_logic_not_compare)
{
  Integer hFFFF_FFFF = Integer("4294967295");
  Integer h1_FFFF_FFFF = Integer("8589934591");
  Integer h1_0000_0000 = Integer("4294967296");
  Integer hFFFF_FFFF_FFFF_FFFF = Integer("18446744073709551615");
  Integer h0000_FFFF_FFFF_0000 = Integer("281474976645120");
  Integer hFFFF_0000_0000_FFFF = Integer("18446462598732906495");
  assert_equal(Integer::Not(hFFFF_FFFF), Integer(0));
  assert_equal(Integer::Not(Integer(0)), hFFFF_FFFF);
  assert_equal(Integer::Not(h1_FFFF_FFFF), Integer(0));
  assert_equal(Integer::Not(h1_0000_0000), hFFFF_FFFF);
  //explicit num_bits
  assert_equal(Integer::Not(hFFFF_FFFF, Integer(33)), h1_0000_0000);
  assert_equal(Integer::Not(Integer(0), Integer(33)), h1_FFFF_FFFF);
  assert_equal(Integer::Not(h1_0000_0000, Integer(33)), hFFFF_FFFF);
  assert_equal(Integer::Not(h1_0000_0000, Integer(15)), Integer(32767));
  assert_equal(Integer::Not(h1_0000_0000, Integer(8)), Integer(255));
  assert_equal(Integer::Not(h1_0000_0000, Integer(4)), Integer(15));
  assert_equal(Integer::Not(h1_0000_0000, Integer(1)), Integer(1));
  assert_equal(Integer::Not(h1_0000_0000, Integer(0)), Integer(0));
  assert_equal(Integer::Not(h0000_FFFF_FFFF_0000, Integer(64)), hFFFF_0000_0000_FFFF);
  assert_equal(Integer::Not(hFFFF_0000_0000_FFFF, Integer(64)), h0000_FFFF_FFFF_0000);
  assert_equal(Integer::Not(hFFFF_FFFF_FFFF_FFFF, Integer(64)), Integer(0));
  assert_equal(Integer::Not(hFFFF_FFFF_FFFF_FFFF, Integer(51)), Integer(0));
  assert_equal(Integer::Not(Integer(0), Integer(64)), hFFFF_FFFF_FFFF_FFFF);
}

//Serialize
static inline void assert_integer_serializes_to(const Integer i, const char *serialization, Integer::Base base = Integer::Base::Decimal)
{
  char buffer[500];
  i.serialize(buffer, 500, base);
  quiz_assert(strcmp(buffer, serialization) == 0);
}

QUIZ_CASE(poincare_logic_serialize)
{
  assert_integer_serializes_to(Integer(-2), "-2");
  assert_integer_serializes_to(Integer("2345678909876"), "2345678909876");
  assert_integer_serializes_to(Integer("-2345678909876"), "-2345678909876");
  assert_integer_serializes_to(Integer(9), "0b1001", Integer::Base::Binary);
  assert_integer_serializes_to(Integer(9131), "0x23AB", Integer::Base::Hexadecimal);
  assert_integer_serializes_to(Integer(123), "123", Integer::Base::Decimal);
  assert_integer_serializes_to(Integer("-2345678909876"), "-2345678909876");
}
