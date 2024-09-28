
#include "manager.h"

#include <QFile>
#include <QGlobalStatic>
#include <QObject>
#include <QTextStream>

Q_GLOBAL_STATIC(Manager, global_manager);

Manager* Manager::get_manager() { return global_manager; }
