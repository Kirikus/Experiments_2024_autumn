#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <QList>

#include "variable_data.h"

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

  void add_measurement_row(int start, int end, QList<double>& measurements);
  void add_measurement_row();
  void remove_measurement_row();
};

#endif  // __MANAGER_H__