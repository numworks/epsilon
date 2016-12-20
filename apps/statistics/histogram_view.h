#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "data.h"
#include "../constant.h"
#include "../curve_view.h"

namespace Statistics {

class HistogramView : public CurveView {
public:
  HistogramView(Data * m_data);
  void reload();
  void initSelectedBin();
  void unselectBin();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void selectNextBinToward(int direction);
  float selectedBin();
private:
  char * label(Axis axis, int index) const override;
  float evaluateCurveAtAbscissa(void * curve, float t) const override;
  Data * m_data;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  float m_selectedBin;
};

}


#endif
