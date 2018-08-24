extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/list_data.h>

namespace Poincare {

ListData::ListData() :
  m_numberOfChildren(0),
  m_operands(new Expression*[0])
{}

ListData::ListData(Expression * operand) :
  m_numberOfChildren(1),
  m_operands(new Expression*[1])
{
  assert(operand != nullptr);
  m_operands[0] = operand;
}

ListData::~ListData() {
  for (int i=0; i<m_numberOfChildren; i++) {
    if (m_operands[i]) {
      delete m_operands[i];
    }
  }
  delete[] m_operands;
}

void ListData::pushExpression(Expression * operand) {
  Expression ** newOperands = new Expression *[m_numberOfChildren+1];
  for (int i=0; i<m_numberOfChildren; i++) {
    newOperands[i] = m_operands[i];
  }
  delete [] m_operands;
  newOperands[m_numberOfChildren] = operand;
  m_operands = newOperands;
  m_numberOfChildren++;
}

int ListData::numberOfChildren() const {
  return m_numberOfChildren;
}

Expression ** ListData::operands() const {
  return m_operands;
}

const Expression * ListData::childAtIndex(int i) const {
  assert(i >= 0);
  assert(i < m_numberOfChildren);
  return m_operands[i];
}

void ListData::detachOperands() {
  for (int i = 0; i < m_numberOfChildren; i++) {
    m_operands[i] = nullptr;
  }
}

}
