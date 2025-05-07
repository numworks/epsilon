#ifndef ESCHER_STACK_VIEW_H
#define ESCHER_STACK_VIEW_H

#include <escher/solid_color_view.h>
#include <escher/stack_header_view.h>
#include <omg/stack.h>

namespace Escher {

class StackView : public View {
 public:
  enum class Style { GrayGradation, PurpleWhite, WhiteUniform };

  using Mask = uint16_t;

  constexpr static uint8_t k_maxDepth = sizeof(Mask) * 8;

  StackView(Style style, bool extendVertically,
            OMG::AbstractStack<StackHeaderView>* headerViewStack);
  int8_t numberOfStacks() const { return m_stackHeaderViews->size(); }
  void setContentView(View* view);
  void setupHeadersBorderOverlaping(bool headersOverlapHeaders,
                                    bool headersOverlapContent,
                                    KDColor headersContentBorderColor);
  void pushStack(const ViewController* controller);
  void reload() { markWholeFrameAsDirty(); }
  void resetStack() { m_stackHeaderViews->reset(); }

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
#endif
 private:
  KDSize minimalSizeForOptimalDisplay() const override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  bool borderShouldOverlapContent() const;

  int numberOfDisplayedHeaders() const;
  // Returns the index in m_stackViews for a given display index
  int stackHeaderIndex(int displayIndex);

  OMG::AbstractStack<StackHeaderView>* m_stackHeaderViews;
  SolidColorView m_borderView;
  View* m_contentView;
  Style m_style;
  // TODO: enum class? Some combination can't exist?
  bool m_headersOverlapHeaders;
  bool m_headersOverlapContent;
  bool m_extendVertically;
};

}  // namespace Escher
#endif
