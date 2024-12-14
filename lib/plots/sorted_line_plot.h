#ifndef __SORTED_LINE_PLOT_H__
#define __SORTED_LINE_PLOT_H__

#include <QList>
#include <QMap>
#include <QPair>
#include <QString>

#include "./ui_sorted_lines_plot.h"
#include "plots.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SortedLinePlot;
}  // namespace Ui
QT_END_NAMESPACE

class SortedLinePlot : public AbstractPlot {
 private:
  QMap<QString, QCPGraph::LineStyle> line_style_map{
      {"None", QCPGraph::lsNone},
      {"Line", QCPGraph::lsLine},
      {"StepLeft", QCPGraph::lsStepLeft},
      {"StepRight", QCPGraph::lsStepRight},
      {"StepCenter", QCPGraph::lsStepCenter},
      {"Impulse", QCPGraph::lsImpulse}};
  QMap<QString, QCPScatterStyle::ScatterShape> scatter_style_map{
      {"None", QCPScatterStyle::ssNone},
      {"Cross", QCPScatterStyle::ssCross},
      {"Plus", QCPScatterStyle::ssPlus},
      {"Circle", QCPScatterStyle::ssCircle},
      {"Disc", QCPScatterStyle::ssDisc},
      {"Square", QCPScatterStyle::ssSquare},
      {"Diamond", QCPScatterStyle::ssDiamond},
      {"Star", QCPScatterStyle::ssStar},
      {"Triangle", QCPScatterStyle::ssTriangle},
      {"TriangleInverted", QCPScatterStyle::ssTriangleInverted},
      {"CrossSquare", QCPScatterStyle::ssCrossSquare},
      {"PlusSquare", QCPScatterStyle::ssPlusSquare},
      {"CrossCircle", QCPScatterStyle::ssCrossCircle},
      {"PlusCircle", QCPScatterStyle::ssPlusCircle},
      {"Peace", QCPScatterStyle::ssPeace}};

  Ui::SortedLinePlot* ui;
  QMap<int, QPair<QList<int>, QList<int>>> variable_to_graph_connection;
  QVector<double> default_numbering_vector;
  QList<XYErrorBars*> bars_list;
  QList<QCPCurve*> bars_ordering;

 public:
  SortedLinePlot(int graph_num = 1, QWidget* parent = nullptr);
  ~SortedLinePlot() { delete ui; }

  // return graph, connect it with ErrorBars and bars_ordering, set all graph's
  // dependencies
  QCPGraph* create_new_graph();

  // return name's index in global Manager
  // -1 if 'None' and -2 if not found
  int get_name_index(QString& name);

 public slots:
  virtual void redraw_settings(int row, int column);

  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>());

  virtual void update_var_names(const QModelIndex& topLeft,
                                const QModelIndex& bottomRight,
                                const QList<int>& roles = QList<int>());
};

#endif