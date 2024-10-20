
#include "error_model.h"

QVariant ErrorModel::data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const {
  if (role == Qt::CheckStateRole) {
    return QVariant();
  }

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

  return QString(*error);
}

bool ErrorModel::setData(const QModelIndex& index, const QVariant& value,
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
      new_error = new ErrorRelative(data / 100);

    } else {
      input.toDouble(success);
      if (input.isEmpty() || !*success) {
        return false;
      }
      data = input.toDouble();
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