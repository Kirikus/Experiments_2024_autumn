#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <QList>

class ErrorData {
 public:
  double data{};
  ErrorData(double data) : data{data} {}
  virtual double getError(double measurement) = 0;
};

class ErrorAbsolute : public ErrorData {
 public:
  using ErrorData::ErrorData;
  double getError(double measurement) override { return data; }
};

class ErrorRelative : public ErrorData {
 public:
  using ErrorData::ErrorData;
  virtual double getError(double measurement) { return measurement * data; }
};

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

  static Manager* get_manager();

  void add_variable(VariableData var) { variables.append(var); }

  // delete last added variable
  void delete_variable() { variables.pop_back(); }

  // clear calculated list
  void clear_calculated() { calculated = QList<VariableData>(); }

  void add_calculated(VariableData var) { calculated.append(var); }

  void add_measurement_row() {
    for (int i = 0; i < variables.size(); ++i) {
      variables[i].measurements.push_back(.0);
      variables[i].errors_local.push_back(nullptr);
    }
  }
  void remove_measurement_row() {
    for (int i = 0; i < variables.size(); ++i) {
      variables[i].measurements.pop_back();
      variables[i].errors_local.pop_back();
    }
  }

 private:
};

#endif  // __MANAGER_H__