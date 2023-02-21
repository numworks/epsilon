#ifndef ESCHER_KEY_VIEW_H
#define ESCHER_KEY_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class KeyView : public TransparentView {
 public:
  enum class Type { Up, Down, Left, Right, Plus, Minus };
  KeyView(Type type = Type::Up);
  void setType(Type type);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_keySize = 8;

 private:
  const uint8_t* mask() const;
  Type m_type;
};

}  // namespace Escher

#endif
