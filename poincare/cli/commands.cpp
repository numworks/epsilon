#include "commands.h"

#include <poincare/expression.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/memory/tree.h>

#include <iostream>

#include "history_context.h"

using namespace Poincare;

HistoryContext s_historyContext;
bool s_isInteractive = false;

Internal::ProjectionContext context() {
  Internal::ProjectionContext ctx;
  ctx.m_context = &s_historyContext;
  ctx.m_symbolic = SymbolicComputation::ReplaceDefinedSymbols;
  return ctx;
}

UserExpression getExpression(const std::vector<std::string>& args) {
  if (args.size() != 1) {
    std::cerr << "This command expects an expression tree\n";
    return Undefined::Builder();
  }
  UserExpression e = Expression::Parse(args[0].c_str(), &s_historyContext);
  if (e.isUninitialized()) {
    std::cerr << "Syntax error\n";
    return Undefined::Builder();
  }
  return e;
}

void printExpression(const UserExpression& expr) {
  if (s_isInteractive) {
    int id = s_historyContext.addOutput(expr);
    std::cout << "o" << id << " = ";
  }
  char buffer[65536];
  expr.serialize(buffer, std::size(buffer));
  std::cout << buffer << std::endl;
}

// Command implementations
void expandCommand(const std::vector<std::string>& args) {
  UserExpression s = getExpression(args);
  Internal::ProjectionContext ctx = context();
  Internal::Tree* p = s.tree()->cloneTree();
  Internal::Simplification::ToSystem(p, &ctx);
  Internal::SystematicReduction::DeepReduce(p);
  Internal::AdvancedReduction::DeepExpand(p);
  Expression r = Expression::Builder(p);
  printExpression(r);
}

void approximateCommand(const std::vector<std::string>& args) {
  UserExpression e = getExpression(args);
  ApproximationContext ctx(&s_historyContext, Preferences::ComplexFormat::Real,
                           Preferences::AngleUnit::Radian);
  printExpression(e.approximateToTree<double>(ctx));
}

void simplifyCommand(const std::vector<std::string>& args) {
  bool reductionFailure = false;
  ProjectionContext ctx = context();
  UserExpression e =
      getExpression(args).cloneAndSimplify(&ctx, &reductionFailure);
  printExpression(e);
}

void logCommand(const std::vector<std::string>& args) {
  UserExpression e = getExpression(args);
  if (s_historyContext.expressionForUserNamed(e)) {
    s_historyContext.expressionForUserNamed(e)->log();
  } else {
    e.tree()->log();
  }
}

void helpCommand(const std::vector<std::string>& args) {
  std::cout << "Available commands (you can use a prefix if unique)\n";
  std::cout << "  approximate     -- Approximate an expression\n";
  std::cout << "  expand          -- Expand an expression using DeepExpand\n";
  std::cout << "  help            -- List available commands\n";
  std::cout << "  log             -- Display a tree\n";
  std::cout
      << "  simplify        -- Simplify an expression with a default context\n";
}

// Command map
std::map<std::string, void (*)(const std::vector<std::string>&)> commands = {
    {"approximate", approximateCommand},
    {"expand", expandCommand},
    {"help", helpCommand},
    {"simplify", simplifyCommand},
    {"log", logCommand},
};
