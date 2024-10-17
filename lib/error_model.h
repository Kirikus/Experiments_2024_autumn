#ifndef __ERRORS_MODEL_H__
#define __ERRORS_MODEL_H__

#include "measurement_model.h"

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

#endif