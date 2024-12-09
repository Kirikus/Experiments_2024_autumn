#include "two_axes_settings_model.h"

#include <QCheckBox>

#include "../data/manager.h"
#include "../delegates.h"

QList<QString> TwoAxesSettingsModel::heading = {
    "Is Active", "Axis_X",  "Axis_Y",       "Style",
    "Color",     "Scatter", "Scatter Size", "Line Size"};

TwoAxesSettingsModel::TwoAxesSettingsModel(QWidget* parent = nullptr)
    : QTableWidget(parent) {
  connect(model(), &QAbstractItemModel::rowsInserted, this,
          &TwoAxesSettingsModel::fillDefaultValues);

  setColumnCount(heading.size());
  setRowCount(1);  //ПОМЕНЯТЬ HARDCODE!!!

  auto* check_box_delegate = new CheckBoxDelegate();
  auto* delegate_variable = new ColumnNameDelegate();
  setItemDelegateForColumn(Is_Active, check_box_delegate);
  setItemDelegateForColumn(Axis_X, delegate_variable);
  setItemDelegateForColumn(Axis_Y, delegate_variable);
  setItemDelegateForColumn(Style, new LineStyleDelegate());
  setItemDelegateForColumn(Color, new ColorDelegate());
  setItemDelegateForColumn(Scatter, new ScatterStyleDelegate());

  setHorizontalHeaderLabels(heading);
}

void TwoAxesSettingsModel::fillDefaultValues(const QModelIndex& parent,
                                             int start, int end) {
  for (int row = start; row <= end; row++) {
    srand(time(NULL));
    setItem(row, Column::Is_Active, new QTableWidgetItem("1"));
    setItem(row, Column::Axis_X, new QTableWidgetItem("None"));
    setItem(row, Column::Axis_Y, new QTableWidgetItem("None"));
    setItem(row, Column::Style, new QTableWidgetItem("Line"));
    setItem(row, Column::Color, new QTableWidgetItem(""));
    QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
    item(row, Column::Color)->setBackground(QBrush(QColor(rgb)));
    setItem(row, Column::Scatter, new QTableWidgetItem("None"));
    setItem(row, Column::Scatter_Size, new QTableWidgetItem("10"));
    setItem(row, Column::Line_Size, new QTableWidgetItem("1"));
  }
}