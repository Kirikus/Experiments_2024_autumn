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
    setRowCount(Manager::get_manager().variables.size());
    setColumnCount(heading.size());

    setItemDelegateForColumn(Is_Active, new CheckBoxDelegate());
    setItemDelegateForColumn(Style, new LineStyleDelegate());
    setItemDelegateForColumn(Color, new ColorDelegate());
    setItemDelegateForColumn(Scatter, new ScatterStyleDelegate());

    int rows_size = rowCount();
    for (int i = 0; i < rows_size; ++i) {
      QTableWidgetItem* item0 = new QTableWidgetItem("0");
      QTableWidgetItem* itemNone = new QTableWidgetItem("Line");
      QTableWidgetItem* itemColor = new QTableWidgetItem("");
      QTableWidgetItem* itemScatter = new QTableWidgetItem("None");
      QTableWidgetItem* itemScatterSize = new QTableWidgetItem("10");
      QTableWidgetItem* itemLineSize = new QTableWidgetItem("2");

      setItem(i, Is_Active, item0);
      setItem(i, Style, itemNone);
      setItem(i, Color, itemColor);
      setItem(i, Scatter, itemScatter);
      setItem(i, Scatter_Size, itemScatterSize);
      setItem(i, Line_Size, itemLineSize);

      QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
      item(i, Color)->setBackground(QBrush(QColor(rgb)));
    }

    setHorizontalHeaderLabels(heading);
  }
};

#endif