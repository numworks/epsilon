#ifndef REGRESSION_OVERRIDEN_TITLE_H
#define REGRESSION_OVERRIDEN_TITLE_H

#include <apps/i18n.h>

namespace Regression {

class OverridenTitle {
public:
  OverridenTitle() : m_title(nullptr) {}
  void setTitle(const char * title) { m_title = title; }

protected:
  const char * getTitle(I18n::Message defaultMessage) const { return m_title ? m_title : I18n::translate(defaultMessage); }
  const char * m_title;
};

}

#endif
