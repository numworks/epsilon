#pragma once

#include <quiz.h>

#include "../helper.h"

void deepSystematicReduce_and_operation_to(
    const Poincare::Internal::Tree* input,
    Poincare::Internal::Tree::Operation operation,
    const Poincare::Internal::Tree* output);
void expand_to(const Poincare::Internal::Tree* input,
               const Poincare::Internal::Tree* output);
void algebraic_expand_to(const Poincare::Internal::Tree* input,
                         const Poincare::Internal::Tree* output);
void contract_to(const Poincare::Internal::Tree* input,
                 const Poincare::Internal::Tree* output);
void reduces_to_tree(const Poincare::Internal::Tree* input,
                     const Poincare::Internal::Tree* output);

void simplifies_to(
    const char* input, const char* output,
    Poincare::Internal::ProjectionContext projectionContext = realCtx,
    bool preserveInput = true);
void projects_and_reduces_to(
    const char* input, const char* output,
    Poincare::Internal::ProjectionContext projectionContext = realCtx);
void simplifies_to_no_beautif(
    const char* input, const char* output,
    Poincare::Internal::ProjectionContext projectionContext = realCtx);
