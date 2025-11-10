#include "helper.h"

QUIZ_CASE(pcj_simplification_boolean) {
  simplifies_to("true", "True");
  simplifies_to("false", "False");
  simplifies_to("True + False", "undef");
  simplifies_to("2True", "undef");
  simplifies_to("False^3", "undef");
  simplifies_to("true and false", "false");
  simplifies_to("1+1=2", "True");
  simplifies_to("2!=3", "True");
  simplifies_to("2<1", "False");
  simplifies_to("1<2<=2", "True");
  simplifies_to("x≥2", "x>=2");
  simplifies_to("x>y>z", "x>y and y>z");
  simplifies_to("a>b≥c=d≤e<f", "a>b and b>=c and c=d and d<=e and e<f");
  simplifies_to("60>5≥1+3=4≤2+2<50", "True");
  simplifies_to("(x>y)>z", "undef");
  simplifies_to("(x and y)>z", "undef");
  simplifies_to("undef<0", "undef");
  simplifies_to("undef<0<1", "undef");
  simplifies_to("{0,1}<1", "{True,False}");
  simplifies_to("1<{1,2,3}<3", "{False,True,False}");
  simplifies_to("1<2<{1,2,3}<4", "{False,False,True}");
  simplifies_to("{undef,undef}<1", "{undef,undef}");
  simplifies_to("{1,2}<undef", "{undef,undef}");
  simplifies_to("0<{undef,undef}<1", "{undef,undef}");
  simplifies_to("{1,2}<3<undef", "{undef,undef}");
  simplifies_to("0<undef<{1,2}", "{undef,undef}");
  simplifies_to("1<i", "undef");
  simplifies_to("not ((i<1) and {True,False})", "{undef,undef}");
  simplifies_to("undef<{0,1}<1", "{undef,undef}");
}

QUIZ_CASE(pcj_simplification_comparison_operators) {
  simplifies_to("3 < 4", "True");
  simplifies_to("3 < 3", "False");
  simplifies_to("3 < 2", "False");

  simplifies_to("3 <= 4", "True");
  simplifies_to("3 <= 3", "True");
  simplifies_to("3 <= 2", "False");

  simplifies_to("3 > 4", "False");
  simplifies_to("3 > 3", "False");
  simplifies_to("3 > 2", "True");

  simplifies_to("3 >= 4", "False");
  simplifies_to("3 >= 3", "True");
  simplifies_to("3 >= 2", "True");

  simplifies_to("3 = 4", "False");
  simplifies_to("3 = 3", "True");
  simplifies_to("3 = 2", "False");

  simplifies_to("3 != 4", "True");
  simplifies_to("3 != 3", "False");
  simplifies_to("3 != 2", "True");

  simplifies_to("undef = 2", "undef");
  simplifies_to("undef != 2", "undef");

  simplifies_to("3 + i < 1 + 2i", "undef");
  simplifies_to("3 + i < 1 + i", "undef");
  simplifies_to("3 + i = 3 + i", "True");
  simplifies_to("[[0, 0]] < [[1, 1]]", "undef");

  simplifies_to("3 > 2 >= 1 = 4 - 3 != 6", "True");
  simplifies_to("3 < 2 >= 1 = 4 - 3 != 6", "False");
  simplifies_to("3 > 2 >= 1 = 4 / 0", "undef");

  simplifies_to("3=3+3<4", "False");
  simplifies_to("(3=3)+(3<4)", "undef");
  simplifies_to("ln(3=5)", "undef");
}

typedef bool (*BoolCompare)(bool a, bool b);
static void testLogicalOperatorTruthTable(const char* operatorString,
                                          BoolCompare evaluationFunction) {
  constexpr const char* booleanNames[] = {"False", "True"};
  constexpr static int bufferSize = 17;  // 9 == strlen("False nand False") + 1
  char buffer[bufferSize];
  int operatorLength = strlen(operatorString);
  assert(operatorLength <= 4);
  // Test truth table
  for (int a = 0; a <= 1; a++) {
    const char* aString = booleanNames[a];
    int length = strlcpy(buffer, aString, strlen(aString) + 1);
    buffer[length] = ' ';
    length++;
    length += strlcpy(buffer + length, operatorString, operatorLength + 1);
    buffer[length] = ' ';
    length++;
    for (int b = 0; b <= 1; b++) {
      const char* bString = booleanNames[b];
      strlcpy(buffer + length, bString, strlen(bString) + 1);
      const char* truthString = booleanNames[evaluationFunction(
          static_cast<bool>(a), static_cast<bool>(b))];
      simplifies_to(buffer, truthString);
    }
  }
  // Test undefined on numbers
  const char* numberString = "1";
  int length = strlcpy(buffer, numberString, strlen(numberString) + 1);
  buffer[length] = ' ';
  length++;
  length += strlcpy(buffer + length, operatorString, operatorLength + 1);
  buffer[length] = ' ';
  length++;
  strlcpy(buffer + length, numberString, strlen(numberString) + 1);
  simplifies_to(buffer, "undef");
}

QUIZ_CASE(pcj_simplification_logical_operators) {
  simplifies_to("not True", "False");
  simplifies_to("not False", "True");
  testLogicalOperatorTruthTable("and", [](bool a, bool b) { return a && b; });
  testLogicalOperatorTruthTable("or", [](bool a, bool b) { return a || b; });
  testLogicalOperatorTruthTable("xor", [](bool a, bool b) { return a != b; });
  testLogicalOperatorTruthTable("nor",
                                [](bool a, bool b) { return !(a || b); });
  testLogicalOperatorTruthTable("nand",
                                [](bool a, bool b) { return !(a && b); });

  simplifies_to("not False and False", "False");
  simplifies_to("not (False and False)", "True");
  simplifies_to("True or False xor True", "False");
  simplifies_to("True or (False xor True)", "True");
  simplifies_to("True xor True and False", "True");
  simplifies_to("(True xor True) and False", "False");

  simplifies_to("True xor {False,True,False,True}", "{True,False,True,False}");

  simplifies_to("True and -5.2", "undef");
  simplifies_to("True and [[-5,2]]", "undef");
  simplifies_to("True or undef", "undef");
  simplifies_to("not undef", "undef");
  simplifies_to("True and 2 = 2 ", "True");
  simplifies_to("(True and 2) = 2", "undef");

  simplifies_to("3×not True", "undef");
}
