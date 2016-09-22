#include "function.h"

Graph::Function::Function() :
  m_text(nullptr),
  m_expression(nullptr),
  m_layout(nullptr)
{
}

Graph::Function::Function(const char * text, KDColor color) :
  m_text(text), // FIXME: Copy !!
  m_name("f(x)"),
  m_color(color),
  m_expression(nullptr),
  m_layout(nullptr),
  m_active(true)
{
}

Graph::Function::~Function() {
  //FIXME: Free m_text!
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression != nullptr) {
    delete m_expression;
  }
}

const char * Graph::Function::text() {
  return m_text;
}

const char * Graph::Function::name() {
  return m_name;
}

Expression * Graph::Function::expression() {
  if (m_expression == nullptr) {
    m_expression = Expression::parse(m_text);
  }
  return m_expression;
}

ExpressionLayout * Graph::Function::layout() {
  if (m_layout == nullptr) {
    m_layout = expression()->createLayout();
  }
  return m_layout;
}

bool Graph::Function::isActive() {
  return m_active;
}

void Graph::Function::setActive(bool active) {
  m_active = active;
}
