#ifndef __MEASUREMENTMODEL_H__
#define __MEASUREMENTMODEL_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"

class MeasurementModel : public QAbstractTableModel {
 protected:
  Manager p_manager;

 public slots:

  void recalculate_data(const QModelIndex& index) {
    std::cout << index.row() << " - " << index.column() << std::endl;
    std::cout << p_manager.variables[index.row()].errors_local[index.column()]
              << std::endl;
  }

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
      p_manager.variables[index.row()].measurements[index.column()] =
          new_value.toDouble();

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

class ErrorModel : public MeasurementModel {
 public:
 private:
  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const {
    int row = index.row();
    int col = index.column();

    ErrorData* error;
    if (!p_manager.variables[index.row()].errors_local[index.column()])
      error = p_manager.variables[index.row()].error_global;
    else {
      error = p_manager.variables[index.row()].errors_local[index.column()];
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
          std::cout << input.toStdString() << 1 << std::endl;
          return false;
        }
        data = input.toDouble();
        if (data < 0) {
          std::cout << 4 << std::endl;
          return false;
        }

        new_error = new ErrorRelative(data / 100);
      } else {
        input.toDouble(success);
        if (input.isEmpty() || !*success) {
          std::cout << 2 << std::endl;
          return false;
        }
        data = input.toDouble();
        if (data < 0) {
          std::cout << 3 << std::endl;
          return false;
        }

        new_error = new ErrorAbsolute(data);
      }

      ErrorData* error;
      if (!p_manager.variables[index.row()].errors_local[index.column()]) {
        error = p_manager.variables[index.row()].error_global;
      } else {
        error = p_manager.variables[index.row()].errors_local[index.column()];
      }

      if (error->data != data) {
        delete p_manager.variables[index.row()].errors_local[index.column()];
        p_manager.variables[index.row()].errors_local[index.column()] =
            new_error;
      }

      std::cout
          << p_manager.variables[index.row()].errors_local[index.column()]->data
          << " ee"
          << "  " << index.row() << " == " << index.column() << std::endl;
      emit dataChanged(index, index, QList({role}));
      std::cout
          << p_manager.variables[index.row()].errors_local[index.column()]->data
          << " fgf"
          << "  " << index.row() << " == " << index.column() << std::endl;

      std::cout << 1 << std::endl;
      return true;
    }
    return false;
  }
};

class MeasurementView : public QTableView {};

#endif
