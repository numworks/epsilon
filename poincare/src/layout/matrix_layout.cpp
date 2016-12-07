#include "matrix_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#define MATRIX_ENTRY_MARGIN 4
#define MATRIX_BRACKET_MARGIN 2
#define MATRIX_BRACKET_WIDTH 2

MatrixLayout::MatrixLayout(ExpressionLayout ** entryLayouts, int numberOfRows, int numberOfColumns) :
  ExpressionLayout(),
  m_entryLayouts(entryLayouts),
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    m_entryLayouts[i]->setParent(this);
  }
  m_baseline = height()/2 + KDText::stringSize(" ").height()/2;
}

MatrixLayout::~MatrixLayout() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    delete m_entryLayouts[i];
  }
  free(m_entryLayouts);
}

KDCoordinate MatrixLayout::rowBaseline(int i) {
  KDCoordinate rowBaseline = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowBaseline = max(rowBaseline, m_entryLayouts[i*m_numberOfColumns+j]->baseline());
  }
  return rowBaseline;
}


KDCoordinate MatrixLayout::rowHeight(int i) {
  KDCoordinate rowHeight = 0;
  KDCoordinate baseline = rowBaseline(i);
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowHeight = max(rowHeight, m_entryLayouts[i*m_numberOfColumns+j]->size().height() - m_entryLayouts[i*m_numberOfColumns+j]->baseline());
  }
  return baseline+rowHeight;
}

KDCoordinate MatrixLayout::height() {
  KDCoordinate totalHeight = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    totalHeight += rowHeight(i);
  }
  totalHeight += (m_numberOfRows-1)*MATRIX_ENTRY_MARGIN;
  return totalHeight;
}

KDCoordinate MatrixLayout::columnWidth(int j) {
  KDCoordinate columnWidth = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    columnWidth = max(columnWidth, m_entryLayouts[i*m_numberOfColumns+j]->size().width());
  }
  return columnWidth;
}

KDCoordinate MatrixLayout::width() {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    totalWidth += columnWidth(j);
  }
  totalWidth += (m_numberOfColumns-1)*MATRIX_ENTRY_MARGIN;
  return totalWidth + 2*MATRIX_BRACKET_WIDTH + 2*MATRIX_BRACKET_MARGIN;
}

void MatrixLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x(), p.y(), MATRIX_BRACKET_WIDTH, height()), expressionColor);
  ctx->fillRect(KDRect(p.x() + width() - MATRIX_BRACKET_WIDTH, p.y(), MATRIX_BRACKET_WIDTH, height()), expressionColor);
}

KDSize MatrixLayout::computeSize() {
  return KDSize(width(), height());
}

ExpressionLayout * MatrixLayout::child(uint16_t index) {
  if (index >= 0 && index < m_numberOfColumns*m_numberOfRows) {
    return m_entryLayouts[index];
  }
  return nullptr;
}

KDPoint MatrixLayout::positionOfChild(ExpressionLayout * child) {
  int rowIndex = 0;
  int columnIndex = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      if (child == m_entryLayouts[i*m_numberOfColumns+j]) {
        rowIndex = i;
        columnIndex = j;
        break;
      }
    }
  }
  KDCoordinate x = 0;
  for (int j = 0; j < columnIndex; j++) {
    x += columnWidth(j);
  }
  x += (columnWidth(columnIndex) - child->size().width())/2+ columnIndex * MATRIX_ENTRY_MARGIN + MATRIX_BRACKET_MARGIN + MATRIX_BRACKET_WIDTH;
  KDCoordinate y = 0;
  for (int i = 0; i < rowIndex; i++) {
    y += rowHeight(i);
  }
  y += rowBaseline(rowIndex) - child->baseline() + rowIndex * MATRIX_ENTRY_MARGIN;
  return KDPoint(x, y);
}
