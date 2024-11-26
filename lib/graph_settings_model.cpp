#include "graph_settings_model.h"

#include <QCheckBox>

#include "delegates.h"
#include "manager.h"

QList<QString> SettingsModel::heading = {
    "Is Active", "Style", "Color", "Scatter", "Scatter Size", "Line Size"};

SettingsModel::SettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
  connect(model(), &QAbstractItemModel::rowsInserted, this,
          &SettingsModel::fillDefaultValues);

  setColumnCount(heading.size());
  setRowCount(Manager::get_manager().variables.size());

  setItemDelegateForColumn(Is_Active, new CheckBoxDelegate());
  setItemDelegateForColumn(Style, new LineStyleDelegate());
  setItemDelegateForColumn(Color, new ColorDelegate());
  setItemDelegateForColumn(Scatter, new ScatterStyleDelegate());

  setHorizontalHeaderLabels(heading);
}

void SettingsModel::fillDefaultValues(const QModelIndex& parent, int start,
                                      int end) {
  for (int row = start; row <= end; row++) {
    setItem(row, Column::Is_Active, new QTableWidgetItem("1"));
    setItem(row, Column::Style, new QTableWidgetItem("Line"));
    setItem(row, Column::Color, new QTableWidgetItem(""));
    QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
    item(row, Column::Color)->setBackground(QBrush(QColor(rgb)));
    setItem(row, Column::Scatter, new QTableWidgetItem("None"));
    setItem(row, Column::Scatter_Size, new QTableWidgetItem("10"));
    setItem(row, Column::Line_Size, new QTableWidgetItem("3"));
  }
}