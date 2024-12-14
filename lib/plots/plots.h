#ifndef __PLOTS_H__
#define __PLOTS_H__

#include <QPen>
#include <QWidget>

#include "qcustomplot.h"

class AbstractPlot : public QWidget {
 public slots:
  // change inner plots' data from Manager
  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>()) = 0;

  // change inner plots' settings from built-in settings model
  virtual void redraw_settings(int row, int column) = 0;

  // update variables' names into Axes choice
  virtual void update_var_names(const QModelIndex& topLeft,
                                const QModelIndex& bottomRight,
                                const QList<int>& roles) = 0;
};

// class for convenient controlling two-dimensional ErrorBars.
// given methods are same as QCPErrorBars'
struct XYErrorBars {
  XYErrorBars(QCPErrorBars* x, QCPErrorBars* y) : x{x}, y{y} {}
  QCPErrorBars* x;
  QCPErrorBars* y;
  void setVisible(bool on) {
    x->setVisible(on);
    y->setVisible(on);
  }

  void setPen(const QPen& pen) {
    x->setPen(pen);
    y->setPen(pen);
  }
};

#endif