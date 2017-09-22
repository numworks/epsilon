#ifndef POINCARE_STATIC_HIERARCHY_H
#define POINCARE_STATIC_HIERARCHY_H

#include <poincare/hierarchy.h>
#include <poincare/list_data.h>

namespace Poincare {

template<int T>
class StaticHierarchy : public Hierarchy {
public:

  StaticHierarchy();
  StaticHierarchy(Expression * const * operands, bool cloneOperands = true);
  ~StaticHierarchy();
  StaticHierarchy(const StaticHierarchy & other) = delete;
  StaticHierarchy(StaticHierarchy && other) = delete;
  StaticHierarchy& operator=(const StaticHierarchy & other) = delete;
  StaticHierarchy& operator=(StaticHierarchy && other) = delete;

  virtual void setArgument(ListData * listData, int numberOfEntries, bool clone);
  int numberOfOperands() const override { return T; }
  bool hasValidNumberOfArguments() const override;
  Expression * const * operands() const override { return m_operands; }
protected:
  void build(Expression * const * operands, int numberOfOperands, bool cloneOperands);
  Expression * m_operands[T];
};

}

#endif
