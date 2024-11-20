#ifndef __VARIABLE_DATA_H__
#define __VARIABLE_DATA_H__

#include <QList>

#include "error_data.h"

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

  auto size() { return measurements.size(); }

  double getMinMeasurement() {
    return *std::min_element(measurements.begin(), measurements.end());
  }

  double getMaxMeasurement() {
    return *std::max_element(measurements.begin(), measurements.end());
  }

  ErrorData* getElemError(int index) {
    ErrorData* error;
    if (!errors_local[index]) {
      error = error_global;
    } else {
      error = errors_local[index];
    }
    return error;
  }

  QString getElemPresentation(int index) {
    double measurement = measurements[index];
    double error = getElemError(index)->getError(measurement);
    if (error == 0) {
      return QString::number(measurement);
    }
    return QString("%1±%2").arg(QString::number(measurement),
                                QString::number(error));
  }
};

#endif