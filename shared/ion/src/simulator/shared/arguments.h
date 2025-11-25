#pragma once

struct Arguments {
#define BOOL_ARG(variable, longForm, shortForm, desc) bool variable = false;
#define TEXT_ARG(variable, longForm, shortForm, desc, argName) \
  const char* variable = nullptr;
#include "arguments.inc"
};

struct ArgumentNames {
#define BOOL_ARG(variable, longForm, shortForm, desc) \
  constexpr static const char* variable = longForm;
#define TEXT_ARG(variable, longForm, shortForm, desc, argName) \
  constexpr static const char* variable = longForm;
#include "arguments.inc"
};
