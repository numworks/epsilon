#ifndef VERTICAL_LAYOUT_H
#define VERTICAL_LAYOUT_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/view.h>
#include <escher/view_controller.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/rect.h>

#include "selectable_cell_list_controller.h"

/*
 * View that lays out its subviews vertically.
 */
template <int numberOfChildren>
class VerticalLayout : public Escher::View {
  // To inherit, just provide the list of subviews, either to the constructor of by calling setView
 public:
  VerticalLayout(Escher::View * views) : m_views{views} {}
  VerticalLayout() : VerticalLayout(nullptr) {}
  int numberOfSubviews() const override { return numberOfChildren; }
  View * subviewAtIndex(int index) override { return m_views[index]; }
  void layoutSubviews(bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setView(Escher::View * v, int i) { m_views[i] = v; }

 protected:
  constexpr static int k_numberOfChildren = numberOfChildren;
  Escher::View * m_views[numberOfChildren];
};

// TODO can it be a view only ?
template <int numberOfChildren>
class VerticalLayoutController : public Escher::ViewController {
 public:
  VerticalLayoutController(Escher::Responder * parent, Escher::View ** subviews)
      : Escher::ViewController(parent), m_contentView(subviews) {}
  VerticalLayoutController(Escher::Responder * parent) : Escher::ViewController(parent){};
  Escher::View * view() override { return &m_contentView; }
  VerticalLayout<numberOfChildren> * verticalLayout() { return &m_contentView; }

 private:
  VerticalLayout<numberOfChildren> m_contentView;
};

template <int n>
void VerticalLayout<n>::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableHeight = frame.height();
  KDRect proposedFrame = KDRect(0, 0, frame.width(), 0);
  int height;
  for (int i = 0; i < n; i++) {
    m_views[i]->setFrame(KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), availableHeight), false);
    height = m_views[i]->minimalSizeForOptimalDisplay().height();
    proposedFrame = KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), height);
    m_views[i]->setFrame(proposedFrame, false);
    availableHeight -= height;
    if (availableHeight < 0) {
      assert(false);
      availableHeight = 0;
    }
  }
}

template <int n>
void VerticalLayout<n>::drawRect(KDContext * ctx, KDRect rectToRedraw) const {
  ctx->fillRect(rectToRedraw, KDColorOrange);
}

class TestVerticalController : public VerticalLayoutController<3> {
 public:
  TestVerticalController(Escher::StackViewController * parent) : VerticalLayoutController<3>(parent), m_list(parent) {
    m_title.setMessage(I18n::Message::ProbaApp);
    m_description.setMessage(I18n::Message::ProbaAppCapital);
    verticalLayout()->setView(&m_title, 0);
    verticalLayout()->setView(&m_description, 1);
    verticalLayout()->setView(m_list.view(), 2);
    m_list.cellAtIndex(0)->setMessage(I18n::Message::ProbaAppCapital);
    m_list.cellAtIndex(1)->setMessage(I18n::Message::ProbaApp);
    m_list.cellAtIndex(2)->setMessage(I18n::Message::ProbaAppCapital);
    m_list.cellAtIndex(3)->setMessage(I18n::Message::ProbaApp);
  }

 private:
  Escher::MessageTextView m_title, m_description;
  SelectableCellListPage<Escher::MessageTableCellWithChevron, 4> m_list;
};

#endif /* VERTICAL_LAYOUT_H */
