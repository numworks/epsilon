#include "function.h"
#include <string.h>

Graph::Function::Function(const char * text, KDColor color) :
  m_name(text),
  m_color(color),
  m_expression(nullptr),
  m_layout(nullptr),
  m_active(true)
{
}

void Graph::Function::setContent(const char * c) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = expression()->createLayout();
}

void Graph::Function::setColor(KDColor color) {
  m_color = color;
}

Graph::Function::~Function() {
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
  return m_expression;
}

ExpressionLayout * Graph::Function::layout() {
  return m_layout;
}

bool Graph::Function::isActive() {
  return m_active;
}

void Graph::Function::setActive(bool active) {
  m_active = active;
}
