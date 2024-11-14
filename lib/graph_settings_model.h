#ifndef __GRAPH_SETTINGS_MODEL_H__
#define __GRAPH_SETTINGS_MODEL_H__

#include <QTableWidget>

#include "manager.h"

class SettingsModel : public QTableWidget {
 public:
  SettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
    setRowCount(Manager::get_manager().variables.size());
    setColumnCount(3);
    setItemDelegateForColumn(1, new LineStyleDelegate());
    setItemDelegateForColumn(2, new ColorDelegate());

    int rows_size = rowCount();
    for (int i = 0; i < rows_size; ++i) {
      QTableWidgetItem* item0 = new QTableWidgetItem("1");
      QTableWidgetItem* itemNone = new QTableWidgetItem("None");
      QTableWidgetItem* itemColor = new QTableWidgetItem("");

      setItem(i, 0, item0);
      setItem(i, 1, itemNone);
      setItem(i, 2, itemColor);
    }

    QTableWidgetItem* item0 = new QTableWidgetItem("Is Active");
    QTableWidgetItem* item1 = new QTableWidgetItem("Style");
    QTableWidgetItem* item2 = new QTableWidgetItem("Color");

    setHorizontalHeaderItem(0, item0);
    setHorizontalHeaderItem(1, item1);
    setHorizontalHeaderItem(2, item2);
  }
};

#endif