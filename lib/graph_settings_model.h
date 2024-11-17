#ifndef __GRAPH_SETTINGS_MODEL_H__
#define __GRAPH_SETTINGS_MODEL_H__

#include <QCheckBox>
#include <QTableWidget>

#include "delegates.h"
#include "manager.h"

class SettingsModel : public QTableWidget {
 public:
  enum Column {
    Is_Active = 0,  // Column for switching graph on/off
    Style,          // Column for line type
    Color,          // Column for graph color
    Scatter,        // Column for scatters type at graph
    Scatter_Size,   // Column for size of scatters
    Line_Size       // Column for line size
  };
  QList<QString> heading{"Is Active", "Style",        "Color",
                         "Scatter",   "Scatter Size", "Line Size"};

  SettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
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

 private slots:
  void fillDefaultValues(const QModelIndex& parent, int start, int end) {
    for (int row = start; row <= end; row++) {
      setItem(row, Column::Is_Active, new QTableWidgetItem("1"));
      setItem(row, Column::Style, new QTableWidgetItem("Line"));
      setItem(row, Column::Color, new QTableWidgetItem(""));
      QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
      item(row, Column::Color)->setBackground(QBrush(QColor(rgb)));
      setItem(row, Column::Scatter, new QTableWidgetItem("None"));
      setItem(row, Column::Scatter_Size, new QTableWidgetItem("10"));
      setItem(row, Column::Line_Size, new QTableWidgetItem("2"));
    }
  }
};

#endif