#ifndef __MANAGER_H__
#define __MANAGER_H__

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

class Manager {
 public:
  QList<VariableData> variables{};
  QList<VariableData> calculated{};

  static Manager& get_manager();

  void add_variable(VariableData var);

  // delete last added variable
  void delete_variable();

  // clear calculated list
  void clear_calculated();

  void add_calculated(VariableData var);

  void add_measurement_row();
  void remove_measurement_row();
};

#endif  // __MANAGER_H__