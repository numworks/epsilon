#ifndef APPS_OMEGA_VIEW_H
#define APPS_OMEGA_VIEW_H

#include <escher.h>

namespace Omega {

class OmegaView : public View {
public:
  OmegaView(SelectableTableView * selectableTableView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews(bool force = false) override;
  MessageTextView m_omegaTextView;
  MessageTextView m_urlTextView;
  MessageTextView m_discordTextView;
  SelectableTableView * m_selectableTableView;
};

}

#endif
