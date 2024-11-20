#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QString>
#include <QTabWidget>

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
    LinePlotType = 0,
    ScatterPlotType,
    HistogramType,
    HeatmapType
  };
  QMap<QString, PlotTypes> plot_type_map{{"Line plot", LinePlotType},
                                         {"Scatter plot", ScatterPlotType},
                                         {"Histogram", HistogramType},
                                         {"Heatmap", HeatmapType}};
  QList<QString> graph_types = {"Line plot", "Scatter plot", "Histogram",
                                "Heatmap"};

 public:
  Ui::DialogWindow* ui;
  QTabWidget* target_tab_widget;
  DialogWindow(QWidget* parent, QTabWidget* target_tab_widget);

 private slots:
  void create_graph();
  void close_dialog();
};