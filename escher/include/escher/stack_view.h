#ifndef ESCHER_STACK_VIEW_H
#define ESCHER_STACK_VIEW_H

#include <escher/stack_header_view.h>
#include <escher/solid_color_view.h>
#include <ion/ring_buffer.h>

namespace Escher {

class StackView : public View {
public:
  enum class Style {
    GrayGradation,
    PurpleWhite,
    WhiteUniform
  };
  static constexpr uint8_t k_maxNumberOfStacks = 7;

  StackView(Style style, bool extendVertically);
  int8_t numberOfStacks() const { return m_stackHeaderViews.length(); }
  void setContentView(View * view);
  void setupHeadersBorderOverlaping(bool headersOverlapHeaders, bool headersOverlapContent, KDColor headersContentBorderColor);
  void pushStack(ViewController * controller);
  void reload() { markRectAsDirty(bounds()); }
  void resetStack() { m_stackHeaderViews.reset(); }
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  KDSize minimalSizeForOptimalDisplay() const override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  bool borderShouldOverlapContent() const;

  int numberOfDisplayedHeaders() const;
  // Returns the index in m_stackViews for a given display index
  int stackHeaderIndex(int displayIndex);

  Ion::RingBuffer<StackHeaderView, k_maxNumberOfStacks> m_stackHeaderViews;
  SolidColorView m_borderView;
  View * m_contentView;
  Style m_style;
  // TODO: enum class? Some combination can't exist?
  bool m_headersOverlapHeaders;
  bool m_headersOverlapContent;
  bool m_extendVertically;
};

}
#endif
