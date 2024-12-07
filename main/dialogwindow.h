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
    OneAxisPlotType = 0,
    TwoAxesPlotType,
    HistogramType,
    HeatmapType
  };
  QMap<QString, PlotTypes> plot_type_map{{"One axis plot", OneAxisPlotType},
                                         {"Two axes plot", TwoAxesPlotType},
                                         {"Histogram", HistogramType},
                                         {"Heatmap", HeatmapType}};
  QList<QString> graph_types = {"One axis plot", "Two axes plot", "Histogram",
                                "Heatmap"};

 public:
  Ui::DialogWindow* ui;
  QTabWidget* target_tab_widget;
  DialogWindow(QWidget* parent, QTabWidget* target_tab_widget);

 private slots:
  void create_graph();
  void close_dialog();
};