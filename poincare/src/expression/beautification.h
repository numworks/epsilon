#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "projection.h"

namespace Poincare::Internal {

namespace Beautification {
float DegreeForSortingAddition(const Tree* e, bool symbolsOnly);
bool AddUnits(Tree* e, ProjectionContext projectionContext);
bool DeepBeautify(Tree* e, ProjectionContext projectionContext = {});

/* Create a Tree to represent a complex value according to the format, for
 * instance 0+1*i => <Constant i> in Cartesian mode. */
template <typename T>
Tree* PushBeautifiedComplex(std::complex<T> value, ComplexFormat complexFormat);

namespace Private {
bool ApplyComplexFormat(Tree* e, Dimension dim,
                        const ProjectionContext& projectionContext);
Tree* GetPolarFormat(const Tree* e, const ProjectionContext& projectionContext);
Tree* GetCartesianFormat(const Tree* e,
                         const ProjectionContext& projectionContext);
bool DeepBeautifyAngleFunctions(Tree* e,
                                const ProjectionContext& projectionContext);
bool BottomUpBeautifyAngleFunctions(Tree* e,
                                    const ProjectionContext& projectionContext,
                                    bool* simplifyParent,
                                    bool canSystematicReduce = true);
bool ShallowBeautifyAngleFunctions(Tree* e, AngleUnit angleUnit,
                                   bool* simplifyParent,
                                   bool canSystematicReduce);
bool ShallowBeautifyPercent(Tree* e);
bool ShallowBeautifyOppositesDivisionsRoots(Tree* e, void* context);
bool ShallowBeautify(Tree* e, void* context);
bool ShallowBeautifySpecialDisplays(Tree* e, void* context);
}  // namespace Private
}  // namespace Beautification

}  // namespace Poincare::Internal

#endif
