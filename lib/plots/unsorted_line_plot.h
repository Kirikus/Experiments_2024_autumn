#ifndef __UNSORTED_LINE_PLOT_H__
#define __UNSORTED_LINE_PLOT_H__

#include <QList>
#include <QMap>
#include <QPair>
#include <QString>

#include "./ui_unsorted_lines_plot.h"
#include "abstract_plot.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class UnsortedLinePlot;
}  // namespace Ui
QT_END_NAMESPACE

class UnsortedLinePlot : public AbstractPlot {
 private:
  Ui::UnsortedLinePlot* ui;
  QMap<int, QPair<QList<int>, QList<int>>> variable_to_graph_connection;
  QVector<double> default_numbering_vector;
  QList<XYErrorBars*> bars_list;
  QList<QCPCurve*> curves;
  QMap<QString, QCPCurve::LineStyle> line_style_map{{"None", QCPCurve::lsNone},
                                                    {"Line", QCPCurve::lsLine}};
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

 public:
  UnsortedLinePlot(int graph_num = 1, QWidget* parent = nullptr);
  ~UnsortedLinePlot() { delete ui; }

  // return graph, connect it with ErrorBars, set all graph's dependencies
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