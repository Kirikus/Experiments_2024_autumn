
#include "manager.h"

#include <QFile>
#include <QGlobalStatic>
#include <QObject>
#include <QTextStream>

Q_GLOBAL_STATIC(Manager, global_manager);

Manager& Manager::get_manager() { return *global_manager; }

void Manager::add_variable(VariableData var) { variables.append(var); }

// delete last added variable
void Manager::delete_variable() { variables.pop_back(); }

// clear calculated list
void Manager::clear_calculated() { calculated = QList<VariableData>(); }

void Manager::add_calculated(VariableData var) { calculated.append(var); }

void Manager::add_measurement_row() {
  for (int i = 0; i < variables.size(); ++i) {
    variables[i].measurements.push_back(.0);
    variables[i].errors_local.push_back(nullptr);
  }
}
void Manager::remove_measurement_row() {
  for (int i = 0; i < variables.size(); ++i) {
    variables[i].measurements.pop_back();
    variables[i].errors_local.pop_back();
  }
}
