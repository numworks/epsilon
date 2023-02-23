#include <quiz.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/layout/constructor.h>
#include <poincare_junior/src/layout/parsing/tokenizer.h>
#include <poincare_junior/src/expression/aliases_list.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_tokenize) {
  ParsingContext context(ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer("ab*123.45"_l, &context);
  Token token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Times && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 6);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::EndOfStream);

  token = Tokenizer("log2"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction && token.length() == 3);

  token = Tokenizer("tantan"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction && token.length() == 3);
}

QUIZ_CASE(pcj_aliases_list) {
  Node layout = "acos"_l;
  RackLayoutDecoder decoder(layout);
  quiz_assert(AliasesLists::k_acosAliases.contains(&decoder));
}
