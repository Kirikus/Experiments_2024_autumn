
#include "error_model.h"

QVariant ErrorModel::data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const {
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    ErrorData* error =
        Manager::get_manager().variables[index.column()].getElemError(
            index.row());
    return QString(*error);
  }

  return QVariant();
}

bool ErrorModel::setData(const QModelIndex& index, const QVariant& value,
                         int role = Qt::EditRole) {
  if (role == Qt::EditRole) {
    QString input = value.toString();
    ErrorData* new_error;
    double data;
    bool success;
    if (input.right(1) == '%') {
      input.chop(1);
      input.toDouble(&success);
      if (input.isEmpty() || !success) {
        return false;
      }
      data = input.toDouble();
      new_error = new ErrorRelative(data / 100);

    } else {
      input.toDouble(&success);
      if (input.isEmpty() || !success) {
        return false;
      }
      data = input.toDouble();
      new_error = new ErrorAbsolute(data);
    }

    ErrorData* error =
        Manager::get_manager().variables[index.column()].getElemError(
            index.row());

    if (error->data != data) {
      delete Manager::get_manager()
          .variables[index.column()]
          .errors_local[index.row()];
      Manager::get_manager()
          .variables[index.column()]
          .errors_local[index.row()] = new_error;
    }

    emit dataChanged(index, index, QList({role}));
    return true;
  }
  return false;
}