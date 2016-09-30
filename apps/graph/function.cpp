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
  #if __GLIBC__
  // FIXME: This is ugly.
  strncpy(m_text, c, sizeof(m_text));
#else
  strlcpy(m_text, c, sizeof(m_text));
#endif
  m_expression = expression();
  m_layout = layout();
}

void Graph::Function::setColor(KDColor color) {
  m_color = color;
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
