#ifndef POINCARE_INTERFACES_H
#define POINCARE_INTERFACES_H

#include <algorithm>
#include "addition_interface.h"
#include "constant_interface.h"
#include "division_interface.h"
#include "ghost_interface.h"
#include "integer_interface.h"
#include "multiplication_interface.h"
#include "power_interface.h"
#include "subtraction_interface.h"

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

constexpr size_t k_maxNumberOfBlocksInNode = std::max({
#if GHOST_REQUIRED
    GhostInterface::k_numberOfBlocksInNode,
#endif
    IntegerInterface::k_minimalNumberOfBlocksInNode,
    AdditionInterface::k_numberOfBlocksInNode,
    MultiplicationInterface::k_numberOfBlocksInNode,
    SubtractionInterface::k_numberOfBlocksInNode,
    DivisionInterface::k_numberOfBlocksInNode,
    PowerInterface::k_numberOfBlocksInNode,
    ConstantInterface::k_numberOfBlocksInNode
  });

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

static constexpr const int k_offsetOfExpressionInterfaces = 1;

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
