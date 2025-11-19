#pragma once

#include <escher/metric.h>

namespace Shared::TableDimensions {

// Width of the row title cells used in statistics result tables
constexpr static int k_rowTitleMaxNumberOfChars = 22;
constexpr static KDCoordinate k_rowTitleCellColumnWidth =
    Escher::Metric::SmallFontCellWidth(
        k_rowTitleMaxNumberOfChars, Escher::Metric::CellVerticalElementMargin);

// Width of the symbol subtitle cells used in statistics result tables
constexpr static int k_symbolColumnMaxNumberOfChars = 3;
constexpr static KDCoordinate k_symbolColumnWidth =
    Escher::Metric::SmallFontCellWidth(
        k_symbolColumnMaxNumberOfChars,
        Escher::Metric::CellVerticalElementMargin);

}  // namespace Shared::TableDimensions
