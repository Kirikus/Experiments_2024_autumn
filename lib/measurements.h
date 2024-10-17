#ifndef __MEASUREMENTMODEL_H__
#define __MEASUREMENTMODEL_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"

class MeasurementModel : public QAbstractTableModel {
 private:
  Manager p_manager;

 public:
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const {
    if (role != Qt::DisplayRole) {
      return QVariant();
    }
    if (orientation == Qt::Vertical) {
      return p_manager.variables[section].short_name;
    } else if (orientation == Qt::Horizontal) {
      return QString::number(section + 1);
    }
  }
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
    int row = index.row();
    int col = index.column();
    double measurement =
        p_manager.variables[index.row()].measurements[index.column()];

    double error = 0;
    if (!p_manager.variables[index.row()].errors_local[index.column()])
      error = fabs(
          p_manager.variables[index.row()].error_global->getError(measurement));
    else {
      error = fabs(p_manager.variables[index.row()]
                       .errors_local[index.column()]
                       ->getError(measurement));
    }

    QString output = QString::number(measurement);
    if (error != 0) {
      output += "±" + QString::number(error);
    }
    return output;
  }
  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole) {
    if (role == Qt::EditRole) {
      if (!value.toBool() or !value.canConvert<double>()) {
        return false;
      }
      p_manager.variables[index.row()].measurements[index.column()] =
          value.toDouble();

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }

  Qt::ItemFlags flags(const QModelIndex& index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  }

  virtual void setManager(Manager& manager) { p_manager = manager; }
};

class MeasurementView : public QTableView {};

#endif
