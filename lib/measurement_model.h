#ifndef __MEASUREMENT_MODEL_H__
#define __MEASUREMENT_MODEL_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"

class MeasurementModel : public QAbstractTableModel {
 public:
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const {
    if (role != Qt::DisplayRole) {
      return QVariant();
    }
    if (orientation == Qt::Vertical) {
      return Manager::get_manager().variables[section].short_name;
    } else if (orientation == Qt::Horizontal) {
      return QString::number(section + 1);
    }
  }

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const {
    return Manager::get_manager().variables.size();
  }

  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const {
    long long int columns = 0;
    for (auto& i : Manager::get_manager().variables) {
      columns = std::max(columns, i.measurements.size());
    }
    return columns;
  }

  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const {
    if (role == Qt::CheckStateRole) {
      return QVariant();
    }
    int row = index.row();
    int col = index.column();
    double measurement = Manager::get_manager()
                             .variables[index.row()]
                             .measurements[index.column()];

    double error = 0;
    if (!Manager::get_manager()
             .variables[index.row()]
             .errors_local[index.column()])
      error = fabs(
          Manager::get_manager().variables[index.row()].error_global->getError(
              measurement));
    else {
      error = fabs(Manager::get_manager()
                       .variables[index.row()]
                       .errors_local[index.column()]
                       ->getError(measurement));
    }

    QString output = QString::number(measurement);
    if (error != 0) {
      return QString("%1±%2").arg(output, QString::number(error));
    }
    return output;
  }

  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole) {
    if (role == Qt::EditRole) {
      QString data = value.toString();
      QVariant new_value;
      if (data.contains("±")) {
        int i = data.indexOf("±");
        data = data.first(i);
        new_value = QVariant(data);
      } else {
        new_value = value;
      }

      if (!new_value.toBool() || !new_value.canConvert<double>()) {
        return false;
      }
      Manager::get_manager()
          .variables[index.row()]
          .measurements[index.column()] = new_value.toDouble();

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }

  Qt::ItemFlags flags(const QModelIndex& index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  }
};

class MeasurementView : public QTableView {};

#endif
