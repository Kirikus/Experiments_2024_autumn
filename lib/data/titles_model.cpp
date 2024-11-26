 #include "titles_model.h"
 
QVariant TitleModel::data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const{
    if ((role != Qt::DisplayRole) && (role != Qt::EditRole)){
        return QVariant();
    }
    QString title = Manager::get_manager()
                                .variables[index.column()].short_name;
    return title;
    }


bool TitleModel::setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole){ 

    if ((role == Qt::EditRole) || (value.toBool())) {
      Manager::get_manager()
          .variables[index.column()].short_name = value.toString();

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }

int TitleModel::rowCount(
    const QModelIndex& parent = QModelIndex()) const{
  return 1;
}