
#include "manager.h"

#include <QFile>
#include <QGlobalStatic>
#include <QObject>
#include <QTextStream>

Q_GLOBAL_STATIC(Manager, global_manager);

Manager& Manager::get_manager() { return *global_manager; }

void Manager::add_variable(VariableData var) { variables.append(var); }

void Manager::delete_variable(int index) { variables.removeAt(index); }

void Manager::clear_calculated() { calculated = QList<VariableData>(); }

void Manager::add_calculated(VariableData var) { calculated.append(var); }

void Manager::add_measurement_row(int start, int end,
                                  QList<double>& measurements) {
  if (end == -1) {
    end = variables.size();
  }
  for (int i = start; i < end; ++i) {
    if (i - start < measurements.size()) {
      variables[i].measurements.push_back(measurements[i - start]);
    } else {
      variables[i].measurements.push_back(0.);
    }
    variables[i].errors_local.push_back(
        nullptr);  // for nullptr will be used general error
  }
}

void Manager::add_measurement_row() {
  for (int i = 0; i < variables.size(); ++i) {
    variables[i].measurements.push_back(.0);
    variables[i].errors_local.push_back(
        nullptr);  // for nullptr will be used general error
  }
}
void Manager::remove_measurement_row(int index) {
  for (int i = 0; i < variables.size(); ++i) {
    variables[i].measurements.removeAt(index);
    variables[i].errors_local.removeAt(index);
  }
}
