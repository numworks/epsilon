#include "commands.h"

#include <poincare/expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

#include <iostream>

// Command implementations
void simplifyCommand(const std::vector<std::string>& args) {
  bool reductionFailure = false;
  Poincare::Internal::ProjectionContext ctx;
  Poincare::Expression e = Poincare::Expression::Parse(args[0].c_str(), nullptr)
                               .cloneAndSimplify(&ctx, &reductionFailure);
  char buffer[1024];
  e.serialize(buffer, std::size(buffer));
  std::cout << buffer << std::endl;
}

void logCommand(const std::vector<std::string>& args) {
  Poincare::Expression e =
      Poincare::Expression::Parse(args[0].c_str(), nullptr);
  e.tree()->log();
}

// Command map
std::map<std::string, void (*)(const std::vector<std::string>&)> commands = {
    {"simplify", simplifyCommand},
    {"log", logCommand},
};
