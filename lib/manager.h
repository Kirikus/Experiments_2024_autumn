#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <QFile>
#include <QList>
#include <QObject>
#include <QTextStream>

class VariableData {
 private:
  QList<double> measurements;
  QString full_name;
  QString short_name;
  // instrument
  QList<double> errors;

 public:
  VariableData(QList<double> measurements, QList<double> errors,
               QString full_name, QString short_name)
      : measurements{measurements},
        errors{errors},
        full_name{full_name},
        short_name{short_name} {}
};

class Manager {
 public:
  QList<VariableData> variables;
  QList<VariableData> calculated;

  void add_variable(VariableData var) { variables.append(var); }

  // delete last added variable
  void delete_variable() {
    variables.pop_back();
  }

  // clear calculated list
  void clear_calculated() {
    calculated = QList<VariableData>();
    return;
  }

  void add_calculated(VariableData var) { calculated.append(var); }

  void add_measurement_row() {}
  void remove_measurement_row() {}

 private:
};

#endif  // __MANAGER_H__