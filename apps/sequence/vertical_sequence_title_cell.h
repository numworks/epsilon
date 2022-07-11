#ifndef APPS_SEQUENCE_VERTICAL_SEQUENCE_TITLE_CELL_H
#define APPS_SEQUENCE_VERTICAL_SEQUENCE_TITLE_CELL_H

#include "sequence_title_cell.h"

namespace Sequence {

/* This class only purpose is to default the orientation to vertical.
 * TODO : Since it is the only place a vertical orientation is used, it could
 * be worth to move all vertical orientation specific code here. */

class VerticalSequenceTitleCell : public SequenceTitleCell {
public:
  VerticalSequenceTitleCell() : SequenceTitleCell(Shared::FunctionTitleCell::Orientation::VerticalIndicator) {}
};

}

#endif
