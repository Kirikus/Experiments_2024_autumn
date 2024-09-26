#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <QList>
#include <QObject>
#include <QFile>
#include <QTextStream>

class VariableData {
private:
    QList<double> measurements;
    QString full_name;
    QString short_name;
    // instrument
    QList<double> errors;
public:
    VariableData(QList<double> measurements, QList<double> errors, QString full_name, QString short_name) :
        measurements{measurements}, errors{errors}, full_name{full_name}, short_name{short_name} {}
};

class Manager {

public:
    QList<VariableData> variables;
    QList<VariableData> calculated;

    void add_variable(VariableData var) {
        variables.append(var);
    }

    void delete_variable() {
    /* delete last added variable */
        variables.pop_back();
    }   

    void clear_calculated() {
    /* clear calculated list */
        calculated = QList<VariableData>();
        return;
    }

    void add_calculated(VariableData var) {
        calculated.append(var);
    }

    void add_measurement_row() {}
    void remove_measurement_row() {}

private:
};

#endif  // __MANAGER_H__