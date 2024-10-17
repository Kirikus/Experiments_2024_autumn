#ifndef __ERRORS_MODEL_H__
#define __ERRORS_MODEL_H__

#include "measurement_model.h"

class ErrorModel : public MeasurementModel {
 private:
  virtual QVariant data(const QModelIndex& index, int role) const;

  virtual bool setData(const QModelIndex& index, const QVariant& value,
                       int role);
};

#endif