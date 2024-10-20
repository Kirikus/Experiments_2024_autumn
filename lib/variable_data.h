#ifndef __VARIABLE_DATA_H__
#define __VARIABLE_DATA_H__

#include <QList>

#include "errors_to_variable.h"

class VariableData {
 public:
  QList<double> measurements{};
  ErrorData* error_global{};         // error for all measurements
  QList<ErrorData*> errors_local{};  // field if we want to specify error for
                                     // some of measurements
  QString full_name{};
  QString short_name{};
  // instrument

 public:
  VariableData(QList<double> measurements, ErrorData* error, QString full_name,
               QString short_name)
      : measurements{measurements},
        error_global{error},
        errors_local{QList<ErrorData*>(measurements.size(), nullptr)},
        full_name{full_name},
        short_name{short_name} {}
  VariableData(QList<double> measurements, QList<ErrorData*> errors,
               QString full_name, QString short_name)
      : measurements{measurements},
        error_global{errors[0]},
        errors_local{errors},
        full_name{full_name},
        short_name{short_name} {}
};

#endif