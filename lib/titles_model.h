#ifndef __TITIES_H__
#define __TITIES_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"
#include "measurement_model.h"

class TitleModel : public MeasurementModel {
 public:

 virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const override{
    if ((role != Qt::DisplayRole) && (role != Qt::EditRole)){
        return QVariant();
    }
    QString title = Manager::get_manager()
                             .variables[index.column()].short_name;
    return title;
  }


    virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role = Qt::EditRole) override{ 

    if ((role == Qt::EditRole) || (value.toBool())) {
      Manager::get_manager()
          .variables[index.column()].short_name = value.toString();

      emit dataChanged(index, index, QList({role}));
      return true;
    }
    return false;
  }

  virtual int rowCount(
    const QModelIndex& parent = QModelIndex()) const override{
  return 1;
}

};

#endif