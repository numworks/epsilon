#ifndef SETTINGS_SELECTABLE_VIEW_WITH_MESSAGES_H
#define SETTINGS_SELECTABLE_VIEW_WITH_MESSAGES_H

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/selectable_table_view.h>

namespace Settings {

class SelectableViewWithMessages : public Escher::View {
 public:
  SelectableViewWithMessages(Escher::SelectableTableView* selectableTableView);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void setMessage(I18n::Message message);
  void reload();

 private:
  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  Escher::SelectableTableView* m_selectableTableView;
  Escher::MessageTextView m_messageTextView;
};

}  // namespace Settings

#endif
