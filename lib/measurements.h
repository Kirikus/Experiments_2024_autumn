#ifndef __MEASUREMENTMODEL_H__
#define __MEASUREMENTMODEL_H__

#include <QAbstractTableModel>
#include <QTableView>

#include "manager.h"

class MeasurementModel : public QAbstractTableModel {
 private:
  Manager p_manager;

 public:
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const {
    return p_manager.variables.size();
  }
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const {
    long long int columns = 0;
    for (auto& i : p_manager.variables) {
      columns = std::max(columns, i.measurements.size());
    }
    return columns;
  }
  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const {
    return p_manager.variables[index.row()].measurements[index.column()];
  }
  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole) {
    p_manager.variables[index.row()].measurements[index.column()] =
        value.toDouble();
    return true;
  }
  virtual void setManager(Manager& manager) { p_manager = manager; }
};

class MeasurementView : public QTableView {
  MeasurementModel* p_model;
  virtual void setModel(MeasurementModel* model) { p_model = model; }

 public:
  MeasurementView(MeasurementModel* model) { setModel(model); }
};

#endif
