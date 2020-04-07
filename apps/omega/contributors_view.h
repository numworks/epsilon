#ifndef APPS_OMEGA_CONTRIBUTORS_VIEW_H
#define APPS_OMEGA_CONTRIBUTORS_VIEW_H

#include <escher.h>

namespace Omega {

class ContributorsView : public View {
public:
  ContributorsView(SelectableTableView * selectableTableView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews(bool force = false) override;
  SelectableTableView * m_selectableTableView;
};

}

#endif
