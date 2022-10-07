#ifndef POINCARE_INTERFACES_H
#define POINCARE_INTERFACES_H

#include <algorithm>
#include "addition.h"
#include "constant.h"
#include "division.h"
#include "ghost.h"
#include "integer.h"
#include "multiplication.h"
#include "power.h"
#include "subtraction.h"

namespace Poincare {

#if GHOST_REQUIRED
static constexpr GhostInterface k_ghostInterface;
#endif
static constexpr IntegerInterface k_integerInterface;
static constexpr IntegerExpressionInterface k_integerExpressionInterface;
static constexpr AdditionInterface k_additionInterface;
static constexpr AdditionExpressionInterface k_additionExpressionInterface;
static constexpr MultiplicationInterface k_multiplicationInterface;
static constexpr MultiplicationExpressionInterface k_multiplicationExpressionInterface;
static constexpr SubtractionInterface k_subtractionInterface;
static constexpr SubtractionExpressionInterface k_subtractionExpressionInterface;
static constexpr DivisionInterface k_divisionInterface;
static constexpr DivisionExpressionInterface k_divisionExpressionInterface;
static constexpr PowerInterface k_powerInterface;
static constexpr PowerExpressionInterface k_powerExpressionInterface;
static constexpr ConstantInterface k_constantInterface;
static constexpr ConstantExpressionInterface k_constantExpressionInterface;

static constexpr const Interface * k_interfaces[] = {
  // Order has to be the same as TypeTreeBlock
#if GHOST_REQUIRED
  &k_ghostInterface,
#endif
  &k_integerInterface,
  &k_integerInterface,
  &k_integerInterface,
  &k_additionInterface,
  &k_multiplicationInterface,
  &k_powerInterface,
  &k_constantInterface,
  &k_subtractionInterface,
  &k_divisionInterface
};

#if GHOST_REQUIRED
static constexpr const int k_offsetOfExpressionInterfaces = 1;
#else
static constexpr const int k_offsetOfExpressionInterfaces = 0;
#endif

static constexpr const InternalExpressionInterface * k_internalExpressionInterfaces[] = {
  // Order has to be the same as TypeTreeBlock
  &k_integerExpressionInterface,
  &k_integerExpressionInterface,
  &k_integerExpressionInterface,
  &k_additionExpressionInterface,
  &k_multiplicationExpressionInterface,
  &k_powerExpressionInterface,
  &k_constantExpressionInterface
};

static constexpr const ExpressionInterface * k_expressionInterfaces[] = {
  &k_subtractionExpressionInterface,
  &k_divisionExpressionInterface,
};

static constexpr const int k_offsetOfLayoutInterfaces = k_offsetOfExpressionInterfaces + (sizeof(k_internalExpressionInterfaces) + sizeof(k_expressionInterfaces)) / sizeof(void *) ;

}

#endif
