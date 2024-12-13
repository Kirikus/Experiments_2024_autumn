#ifndef __DIALOGWINDOW_H__
#define __DIALOGWINDOW_H__

#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QString>
#include <QTabWidget>

#include "../lib/data/error_model.h"
#include "../lib/data/measurement_model.h"
#include "../lib/data/titles_model.h"
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogWindow;
}
QT_END_NAMESPACE

class DialogWindow : public QDialog {
  Q_OBJECT
 private:
  enum PlotTypes {
    UnsortedLinePlotType = 0,
    SortedLinePlotType,
    HistogramType,
    HeatmapType
  };
  QMap<QString, PlotTypes> plot_type_map{{"Unsorted line plot", UnsortedLinePlotType},
                                         {"Sorted line plot", SortedLinePlotType},
                                         {"Histogram", HistogramType},
                                         {"Heatmap", HeatmapType}};
  QList<QString> graph_types = {"Unsorted line plot", "Sorted line plot", "Histogram",
                                "Heatmap"};
  MeasurementModel* measure_model;
  ErrorModel* err_model;
  TitleModel* titles_model;

 public:
  Ui::DialogWindow* ui;
  QTabWidget* target_tab_widget;
  DialogWindow(QTabWidget* target_tab_widget, MeasurementModel* meas_model,
               ErrorModel* err_model, TitleModel* titles_model,
               QWidget* parent);

 private slots:
  void create_graph();
  void close_dialog();
};

#endif