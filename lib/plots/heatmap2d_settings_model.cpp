#include "heatmap2d_settings_model.h"

#include <QCheckBox>

#include "../data/manager.h"
#include "../delegates.h"

QList<QString> Heatmap2dSettingsModel::heading = {
    "Axis_X", "Axis_Y", "Distribution", "Gradient", "Interpolate"};

Heatmap2dSettingsModel::Heatmap2dSettingsModel(QWidget* parent = nullptr)
    : QTableWidget(parent) {
  setColumnCount(heading.size());
  setRowCount(1);

  auto* check_box_delegate = new CheckBoxDelegate();
  auto* delegate_variable = new ColumnNameDelegate();
  setItemDelegateForColumn(Axis_X, delegate_variable);
  setItemDelegateForColumn(Axis_Y, delegate_variable);
  setItemDelegateForColumn(Gradient, new GradientStyleDelegate());
  setItemDelegateForColumn(Interpolate, check_box_delegate);

  setHorizontalHeaderLabels(heading);

  setItem(0, Column::Axis_X, new QTableWidgetItem("None"));
  setItem(0, Column::Axis_Y, new QTableWidgetItem("None"));
  setItem(0, Column::Distribution, new QTableWidgetItem("1"));
  setItem(0, Column::Gradient, new QTableWidgetItem("Polar"));
  setItem(0, Column::Interpolate, new QTableWidgetItem("1"));
}