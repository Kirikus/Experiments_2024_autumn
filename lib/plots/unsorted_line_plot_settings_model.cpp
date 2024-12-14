#include "unsorted_line_plot_settings_model.h"

#include "../delegates.h"

QList<QString> UnsortedLinePlotSettingsModel::heading = {
    "Is Active",     "Axis_X",  "Axis_Y",       "Line",     "Color",
    "Error Scatter", "Scatter", "Scatter Size", "Line Size"};

UnsortedLinePlotSettingsModel::UnsortedLinePlotSettingsModel(
    QWidget* parent = nullptr)
    : QTableWidget(parent) {
  srand(time(NULL));
  connect(model(), &QAbstractItemModel::rowsInserted, this,
          &UnsortedLinePlotSettingsModel::fillDefaultValues);

  setColumnCount(heading.size());

  auto* check_box_delegate = new CheckBoxDelegate();
  auto* delegate_variable = new ColumnNameDelegate();
  setItemDelegateForColumn(Is_Active, check_box_delegate);
  setItemDelegateForColumn(Axis_X, delegate_variable);
  setItemDelegateForColumn(Axis_Y, delegate_variable);
  setItemDelegateForColumn(Line, check_box_delegate);
  setItemDelegateForColumn(Color, new ColorDelegate());
  setItemDelegateForColumn(Error_Scatter, check_box_delegate);
  setItemDelegateForColumn(Scatter, new ScatterStyleDelegate());

  setHorizontalHeaderLabels(heading);
}

void UnsortedLinePlotSettingsModel::fillDefaultValues(const QModelIndex& parent,
                                                      int start, int end) {
  for (int row = start; row <= end; row++) {
    setItem(row, Column::Is_Active, new QTableWidgetItem("1"));
    setItem(row, Column::Axis_X, new QTableWidgetItem("None"));
    setItem(row, Column::Axis_Y, new QTableWidgetItem("None"));
    setItem(row, Column::Line, new QTableWidgetItem("1"));
    setItem(row, Column::Color, new QTableWidgetItem(""));
    QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
    item(row, Column::Color)->setBackground(QBrush(QColor(rgb)));
    setItem(row, Column::Error_Scatter, new QTableWidgetItem("1"));
    setItem(row, Column::Scatter, new QTableWidgetItem("None"));
    setItem(row, Column::Scatter_Size, new QTableWidgetItem("10"));
    setItem(row, Column::Line_Size, new QTableWidgetItem("1"));
  }
}