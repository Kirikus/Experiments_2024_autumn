#include "one_axis_settings_model.h"

#include <QCheckBox>

#include "../data/manager.h"
#include "../delegates.h"

QList<QString> OneAxisSettingsModel::heading = {
    "Is Active", "Style", "Color", "Error Scatter", "Scatter", "Scatter Size", "Line Size"};

OneAxisSettingsModel::OneAxisSettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
  srand(time(NULL));
  connect(model(), &QAbstractItemModel::rowsInserted, this,
          &OneAxisSettingsModel::fillDefaultValues);

  setColumnCount(heading.size());
  setRowCount(Manager::get_manager().variables.size());

  setItemDelegateForColumn(Is_Active, new CheckBoxDelegate());
  setItemDelegateForColumn(Style, new LineStyleDelegate());
  setItemDelegateForColumn(Color, new ColorDelegate());
  setItemDelegateForColumn(Error_Scatter, new CheckBoxDelegate());
  setItemDelegateForColumn(Error_Scatter, new CheckBoxDelegate());
  setItemDelegateForColumn(Scatter, new ScatterStyleDelegate());

  setHorizontalHeaderLabels(heading);
}

void OneAxisSettingsModel::fillDefaultValues(const QModelIndex& parent, int start,
                                      int end) {
  for (int row = start; row <= end; row++) {
    setItem(row, Column::Is_Active, new QTableWidgetItem("1"));
    setItem(row, Column::Style, new QTableWidgetItem("Line"));
    setItem(row, Column::Color, new QTableWidgetItem(""));
    QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
    item(row, Column::Color)->setBackground(QBrush(QColor(rgb)));
    setItem(row, Column::Error_Scatter, new QTableWidgetItem("1"));
    setItem(row, Column::Error_Scatter, new QTableWidgetItem("1"));
    setItem(row, Column::Scatter, new QTableWidgetItem("None"));
    setItem(row, Column::Scatter_Size, new QTableWidgetItem("10"));
    setItem(row, Column::Line_Size, new QTableWidgetItem("1"));
    setItem(row, Column::Line_Size, new QTableWidgetItem("1"));
  }
}