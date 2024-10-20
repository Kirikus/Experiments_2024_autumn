
#include "measurement_model.h"

QVariant MeasurementModel::headerData(int section, Qt::Orientation orientation,
                                      int role = Qt::DisplayRole) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      return Manager::get_manager().variables[section].short_name;
    } else if (orientation == Qt::Vertical) {
      return QString::number(section + 1);
    }
  }
  return QVariant();
}

int MeasurementModel::columnCount(
    const QModelIndex& parent = QModelIndex()) const {
  return Manager::get_manager().variables.size();
}

int MeasurementModel::rowCount(
    const QModelIndex& parent = QModelIndex()) const {
  long long int row = 0;
  for (auto& i : Manager::get_manager().variables) {
    row = std::max(row, i.measurements.size());
  }
  return row;
}

QVariant MeasurementModel::data(const QModelIndex& index,
                                int role = Qt::DisplayRole) const {
  VariableData row = Manager::get_manager().variables[index.column()];
  if (role == Qt::DisplayRole) {
    return row.getElemPresentation(index.row());
  } else if (role == Qt::EditRole) {
    return row.measurements[index.row()];
  }
  return QVariant();
}

bool MeasurementModel::setData(const QModelIndex& index, const QVariant& value,
                               int role = Qt::EditRole) {
  if (role == Qt::EditRole) {
    if (!value.toBool() || !value.canConvert<double>()) {
      return false;
    }
    Manager::get_manager().variables[index.column()].measurements[index.row()] =
        value.toDouble();

    emit dataChanged(index, index, QList({role}));
    return true;
  }
  return false;
}

Qt::ItemFlags MeasurementModel::flags(const QModelIndex& index) const {
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
