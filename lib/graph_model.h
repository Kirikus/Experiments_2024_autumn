#ifndef __GRAPH_MODEL_H__
#define __GRAPH_MODEL_H__

#include <QList>
#include <QString>
#include <QStyledItemDelegate>
#include <QTableWidget>

#include "manager.h"
#include "measurement_model.h"
#include "qcustomplot.h"

class ColorDelegate : QStyledItemDelegate {
 public:
  ColorDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    drawBackground(painter, option, index);
    QStyledItemDelegate::paint(painter, option, index);
  }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    return new QColorDialog(parent);
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QColorDialog* color_dialog = static_cast<QColorDialog*>(editor);
    model->setData(index, color_dialog->selectedColor(), Qt::EditRole);
  }

  // virtual QSize sizeHint() const { return QSize(); }  // for QAbstractItemDelegate

 protected:
  virtual void drawBackground(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const {
    painter->fillRect(option.rect, index.model()->data(index).value<QColor>());
  }
};

class SettingsModel : public QTableWidget {
 public:
  SettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
    setRowCount(Manager::get_manager().variables.size());
    setColumnCount(3);
    setItemDelegateForColumn(2, new ColorDelegate());

    int rows_size = rowCount();
    for (int i = 0; i < rows_size; ++i) {
      QTableWidgetItem* item0 = new QTableWidgetItem("0");
      QTableWidgetItem* itemNone = new QTableWidgetItem("None");
      QTableWidgetItem* itemColor = new QTableWidgetItem("Color0");

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

  // virtual bool setData(const QModelIndex& index, const QVariant& value,
  //                      int role) {
  //   if (role == Qt::EditRole) {
  //     data[index.row()][index.column()] = value;
  //     emit dataChanged(index, index, QList({role}));
  //     return true;
  //   }
  // }

  // Qt::ItemFlags flags(const QModelIndex& index) const {
  //   return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  // }
};

class AbstractPlot {
 public:
  virtual void draw() = 0;
  QSplitter* main_widget;

 protected:
  void create_environment(QWidget* parent = nullptr) {
    QCustomPlot* graph = new QCustomPlot();
    QTableWidget* table_settings = new SettingsModel();
    QSplitter* splitter = new QSplitter(Qt::Vertical, parent);
    splitter->addWidget(graph);
    splitter->addWidget(table_settings);
    main_widget = splitter;
  }
};

class ScatterPlot : public AbstractPlot {
 public:
  ScatterPlot(QWidget* parent = nullptr) { create_environment(parent); }
  virtual void draw() {}  // main_widget->widget(0)->draw();
};

// class GraphModel : public QAbstractTableModel {

// }

// class SettingsItemDelegate : QStyledItemDelegate {
//   virtual QWidget* createEditor(QWidget* parent,
//                                 const QStyleOptionViewItem& option,
//                                 const QModelIndex& index) const {
//     return QStyledItemDelegate::createEditor(parent, option, index);
//   }
// }

#endif