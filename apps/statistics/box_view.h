#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include <escher.h>
#include "data.h"
#include "box_window.h"
#include "../constant.h"
#include "../curve_view.h"

namespace Statistics {

class BoxView : public CurveView {
public:
  BoxView(Data * m_data);
  void reload(int selectedQuantile);
  int selectedQuantile();
  bool selectQuantile(int selectedQuantile);
  bool isAnyQuantileSelected();
  void selectAnyQuantile(bool selectAnyQuantile);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  float evaluateCurveAtAbscissa(void * curve, float t) const override;
  Data * m_data;
  BoxWindow m_boxWindow;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  // -1->Unselect 0->min 1->first quartile 2->median 3->third quartile 4->max
  bool m_anyQuantileSelected;
  int m_selectedQuantile;
};

}


#endif
