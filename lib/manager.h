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
  QList<ErrorData*> errors{};

 private:
  QString full_name{};
  QString short_name{};
  // instrument

 public:
  VariableData(QList<double> measurements, QList<ErrorData*> errors_list,
               QString full_name, QString short_name)
      : measurements{measurements},
        full_name{full_name},
        short_name{short_name} {
    ErrorData* standart_err = new ErrorAbsolute(0.);
    errors = QList<ErrorData*>(measurements.size(), standart_err);
    for (int i = 0; i < errors_list.size(); ++i) {
      errors[i] = errors_list[i];
    }
  }
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
      ErrorData* standart_err = new ErrorAbsolute(0.);
      variables[i].errors.push_back(standart_err);
    }
  }
  void remove_measurement_row() {
    for (int i = 0; i < variables.size(); ++i) {
      variables[i].measurements.pop_back();
      variables[i].errors.pop_back();
    }
  }

 private:
};

#endif  // __MANAGER_H__