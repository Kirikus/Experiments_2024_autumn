#ifndef __TITLES_H__
#define __TITLES_H__

#include <QAbstractTableModel>
#include <QString>
#include <QTableView>

#include "manager.h"
#include "measurement_model.h"

class TitleModel : public MeasurementModel {
 public:
  virtual QVariant data(const QModelIndex& index,
                        int role) const override;

  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role) override;

  virtual int rowCount(const QModelIndex& parent) const override;
};

#endif