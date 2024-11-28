 #include "titles_model.h"
 
QVariant TitleModel::headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const {
  // if (role == Qt::DisplayRole) {
  //   if (orientation == Qt::Horizontal) {
  //     return QString();
  //   } else if (orientation == Qt::Vertical) {
  //     return QVariant();
  //   }
  // }
  return QVariant();
}

QVariant TitleModel::data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const{
    if ((role != Qt::DisplayRole) && (role != Qt::EditRole)){
        return QVariant();
    }
    QString title = Manager::get_manager()
                                .variables[index.row()].short_name;
    return title;
    }


bool TitleModel::setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole){ 
    if ((role == Qt::EditRole) || (value.toBool())) {
      Manager::get_manager()
          .variables[index.row()].short_name = value.toString();

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }

int TitleModel::columnCount(
    const QModelIndex& parent = QModelIndex()) const{
  return 1;
}

int TitleModel::rowCount(
    const QModelIndex& parent = QModelIndex()) const{
  return Manager::get_manager().variables.size();
}