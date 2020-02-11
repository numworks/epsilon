#ifndef SETTINGS_SELECTABLE_VIEW_WITH_MESSAGES_H
#define SETTINGS_SELECTABLE_VIEW_WITH_MESSAGES_H

#include <escher/selectable_table_view.h>
#include <apps/i18n.h>

namespace Settings {

class SelectableViewWithMessages : public View {
public:
  SelectableViewWithMessages(SelectableTableView * selectableTableView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setMessages(I18n::Message * messages, int numberOfMessages);
  void reload();
private:
  int numberOfSubviews() const override { return 1 + m_numberOfMessages; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  SelectableTableView * m_selectableTableView;
  static constexpr int k_maxNumberOfLines = 4;
  MessageTextView m_messageLines[k_maxNumberOfLines];
  int m_numberOfMessages;
};

}

#endif
