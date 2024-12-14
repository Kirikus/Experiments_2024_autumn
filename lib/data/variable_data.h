#ifndef __VARIABLE_DATA_H__
#define __VARIABLE_DATA_H__

#include <QList>

#include "error_data.h"

class VariableData {
 public:
  QList<double> measurements{};
  ErrorData* error_global{};  // error for all measurements
  QList<ErrorData*>
      errors_local{};  // specifying error for some of measurements
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

  QList<double> getErrors() {
    QList<double> errors(measurements.size());
    for (int i = 0; i < measurements.size(); ++i) {
      errors[i] = getElemError(i)->getError(measurements[i]);
    }
    return errors;
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

  double get_segment_fraction(double start, double end) {
    double count = 0.;
    for (double elem : measurements) {
      if (start <= elem && elem < end) {
        ++count;
      }
    }
    return count / measurements.size();
  }

  QVector<double> get_distribution(int partition) {
    double min_measurement = getMinMeasurement();
    double step = (getMaxMeasurement() - min_measurement) / partition;
    QVector<double> result = QVector<double>(partition);
    for (int i = 0; i < partition - 1; ++i) {
      result[i] = get_segment_fraction(min_measurement + step * i,
                                       min_measurement + step * (i + 1));
    }
    return result;
  }
};

#endif