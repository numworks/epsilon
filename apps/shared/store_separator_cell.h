#ifndef APPS_SHARED_STORE_SEPARATOR_CELL_H
#define APPS_SHARED_STORE_SEPARATOR_CELL_H

namespace Shared {

class StoreSeparatorCell {
public:
  StoreSeparatorCell() :
    m_separatorRight(false)
  {}
  bool separatorRight() const { return m_separatorRight; }
  virtual void setSeparatorRight(bool separator) { m_separatorRight = separator; }
protected:
  static constexpr KDCoordinate k_separatorThickness = 2;
private:
  bool m_separatorRight;
};

}

#endif
