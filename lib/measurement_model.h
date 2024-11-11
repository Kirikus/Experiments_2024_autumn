#ifndef __MEASUREMENT_MODEL_H__
#define __MEASUREMENT_MODEL_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"

class MeasurementModel : public QAbstractTableModel {
 public:
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role) const;

  virtual int rowCount(const QModelIndex& parent) const;

  virtual int columnCount(const QModelIndex& parent) const;

  virtual QVariant data(const QModelIndex& index, int role) const;

  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role);

  Qt::ItemFlags flags(const QModelIndex& index) const;

  void keyPressEvent(QKeyEvent* event) const;
};

class MeasurementView : public QTableView {};

#endif
