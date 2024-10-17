#ifndef __MEASUREMENTMODEL_H__
#define __MEASUREMENTMODEL_H__

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

class ErrorModel : public MeasurementModel {
 public:
 private:
  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const {
    int row = index.row();
    int col = index.column();

    ErrorData* error;
    if (!Manager::get_manager()
             .variables[index.row()]
             .errors_local[index.column()])
      error = Manager::get_manager().variables[index.row()].error_global;
    else {
      error = Manager::get_manager()
                  .variables[index.row()]
                  .errors_local[index.column()];
    }

    if (error->getError(1) !=
        error->getError(2)) {  // check: *error != ErrorAbsolute
      return QString("%1%").arg(error->data * 100);
    }

    return error->data;
  }

  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole) {
    if (role == Qt::EditRole) {
      QString input = value.toString();
      ErrorData* new_error;
      double data;
      bool* success;
      if (input.right(1) == '%') {
        input.chop(1);
        input.toDouble(success);
        if (input.isEmpty() || !*success) {
          return false;
        }
        data = input.toDouble();
        if (data < 0) {
          return false;
        }

        new_error = new ErrorRelative(data / 100);
      } else {
        input.toDouble(success);
        if (input.isEmpty() || !*success) {
          return false;
        }
        data = input.toDouble();
        if (data < 0) {
          return false;
        }

        new_error = new ErrorAbsolute(data);
      }

      ErrorData* error;
      if (!Manager::get_manager()
               .variables[index.row()]
               .errors_local[index.column()]) {
        error = Manager::get_manager().variables[index.row()].error_global;
      } else {
        error = Manager::get_manager()
                    .variables[index.row()]
                    .errors_local[index.column()];
      }

      if (error->data != data) {
        delete Manager::get_manager()
            .variables[index.row()]
            .errors_local[index.column()];
        Manager::get_manager()
            .variables[index.row()]
            .errors_local[index.column()] = new_error;
      }

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }
};

class MeasurementView : public QTableView {};

#endif
