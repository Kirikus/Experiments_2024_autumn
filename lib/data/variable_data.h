#ifndef __VARIABLE_DATA_H__
#define __VARIABLE_DATA_H__

#include <QList>

#include "error_data.h"

class VariableData {
 public:
  QList<double> measurements{};
  ErrorData* error_global{};  // general error for all measurements
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

  // return list with calculated error values
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

  // purified element view: 'measurement±error'
  QString getElemPresentation(int index) {
    double measurement = measurements[index];
    double error = getElemError(index)->getError(measurement);
    if (error == 0) {
      return QString::number(measurement);
    }
    return QString("%1±%2").arg(QString::number(measurement),
                                QString::number(error));
  }

  // return fraction of measurements in a given interval
  // 0 <= get_segment_fraction() <= 1
  double get_segment_fraction(double start, double end) {
    double count = 0.;
    for (double elem : measurements) {
      if (start <= elem && elem < end) {
        ++count;
      }
    }
    return count / measurements.size();
  }

  // [min_measurement, max_measurement] is divided into 'int partition' segments
  // returns list of proportions of measurements in every segment
  // result.size() = partition
  QVector<double> get_distribution(int partition) {
    double min_measurement = getMinMeasurement() - 0.000001;
    double step = (getMaxMeasurement() - min_measurement) / partition;
    QVector<double> result = QVector<double>(partition);
    for (int i = 0; i < partition; ++i) {
      result[i] = get_segment_fraction(
          min_measurement + step * i,
          min_measurement + step * (i + 1) + 0.000001 * (i == partition - 1));
    }
    // get a bit wider range for max
    // boundary because of [min, max) != [min, max]
    return result;
  }
};

#endif