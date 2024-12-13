#ifndef __PLOTS_H__
#define __PLOTS_H__

#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPen>
#include <QString>
#include <QWidget>
#include <limits>

#include "../data/manager.h"
#include "../data/measurement_model.h"
#include "../delegates.h"
#include "./ui_sorted_lines_plot.h"
#include "./ui_unsorted_lines_plot.h"
#include "qcustomplot.h"
#include "two_axes_settings_model.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SortedLinePlot;
class UnsortedLinePlot;
}  // namespace Ui
QT_END_NAMESPACE

class AbstractPlot : public QWidget {
 public:
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

 public slots:
  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>()) = 0;
  virtual void redraw_settings(int row, int column) = 0;
};

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

class UnsortedLinePlot : public AbstractPlot {
 private:
  Ui::UnsortedLinePlot* ui;
  QMap<int, QPair<QList<int>, QList<int>>> variable_to_graph_connection;
  QVector<double> default_numbering_vector;
  QList<XYErrorBars*> bars_list;

 public:
  UnsortedLinePlot(int graph_num = 1, QWidget* parent = nullptr)
      : ui(new Ui::UnsortedLinePlot) {
    ui->setupUi(this);

    ui->plot->xAxis->setLabel("Axis X");
    ui->plot->yAxis->setLabel("Axis Y");
    ui->settings->setRowCount(graph_num);
    int rows_count = ui->settings->rowCount();

    for (int i = 0; i < Manager::get_manager().variables[0].measurements.size();
         ++i) {
      default_numbering_vector.append(i + 1);
    }

    connect(ui->settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto manager_variables = Manager::get_manager().variables;
    auto manager_line_x = manager_variables[0];
    auto manager_line_y = manager_variables[0];
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_x = manager_line_x.measurements[0];
    double max_x = manager_line_x.measurements[0];
    double min_y = manager_line_y.measurements[0];
    double max_y = manager_line_y.measurements[0];

    variable_to_graph_connection[0] =
        QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
    for (int i = 0; i < graph_num; ++i) {
      variable_to_graph_connection[0].first.append(i);
      variable_to_graph_connection[0].second.append(i);
    }
    for (int i = 0; i < manager_variables.size(); ++i) {
      variable_to_graph_connection[i + 1] =
          QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
      static_cast<ColumnNameDelegate*>(
          ui->settings->itemDelegateForColumn(
              TwoAxesSettingsModel::Column::Axis_X))
          ->get_options_list()
          .append(manager_variables[i].short_name);
    }

    for (int i = 0; i < graph_num; ++i) {
      is_active = ui->settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = create_new_graph();

      if (is_active) {
        min_x = std::min(min_x, manager_line_x.getMinMeasurement());
        max_x = std::max(max_x, manager_line_x.getMaxMeasurement());
        min_y = std::min(min_y, manager_line_y.getMinMeasurement());
        max_y = std::max(max_y, manager_line_y.getMaxMeasurement());
      }
    }

    update_data(ui->settings->model()->index(-1, -1),
                ui->settings->model()->index(-1, -1));
    for (int i = 0; i < graph_num; ++i) {
      for (int k : QList({TwoAxesSettingsModel::Column::Is_Active,
                          TwoAxesSettingsModel::Column::Axis_X,
                          TwoAxesSettingsModel::Column::Style,
                          TwoAxesSettingsModel::Column::Scatter_Size,
                          TwoAxesSettingsModel::Column::Line_Size})) {
        redraw_settings(i, k);
      }
    }
    ui->plot->xAxis->setRange(min_x - (max_x - min_x) / 20.,
                              max_x + (max_x - min_x) / 20.);
    ui->plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                              max_y + (max_y - min_y) / 20.);

    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->replot();
  }
  ~UnsortedLinePlot() { delete ui; }

  QCPGraph* create_new_graph() {
    auto graph = ui->plot->addGraph();
    QCPErrorBars* errorBars_x =
        new QCPErrorBars(ui->plot->xAxis, ui->plot->yAxis);
    QCPErrorBars* errorBars_y =
        new QCPErrorBars(ui->plot->yAxis, ui->plot->xAxis);
    errorBars_y->setDataPlottable(graph);
    errorBars_x->setDataPlottable(graph);
    bars_list.append(new XYErrorBars(errorBars_x, errorBars_y));
    return graph;
  }

  int get_name_index(QString& name) {
    if (name == "None") {
      return -1;
    }
    for (int i = 0; i < Manager::get_manager().variables.size(); ++i) {
      if (name == Manager::get_manager().variables[i].short_name) {
        return i;
      }
    }
    return -2;
  }

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = ui->plot->graph(row);

    switch (column) {
      case TwoAxesSettingsModel::Column::Axis_X:
      case TwoAxesSettingsModel::Column::Axis_Y: {
        auto& man = Manager::get_manager();

        QString name_x =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_X)
                ->data(Qt::DisplayRole)
                .value<QString>();
        QString name_y =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_Y)
                ->data(Qt::DisplayRole)
                .value<QString>();
        auto name = ui->settings->item(row, column)
                        ->data(Qt::DisplayRole)
                        .value<QString>();
        int name_x_ind = get_name_index(name_x);
        int name_y_ind = get_name_index(name_y);
        int name_ind = get_name_index(name);

        int ind_remove_x;
        int ind_remove_y;
        for (auto& elems : variable_to_graph_connection) {
          ind_remove_x = elems.first.indexOf(row);
          ind_remove_y = elems.second.indexOf(row);
          if (ind_remove_x != -1) {
            elems.first.removeAt(ind_remove_x);
          }
          if (ind_remove_y != -1) {
            elems.second.removeAt(ind_remove_y);
          }
        }
        int x = get_name_index(name_x) + 1;
        int y = get_name_index(name_y) + 1;
        variable_to_graph_connection[x].first.append(row);
        variable_to_graph_connection[y].second.append(row);

        ColumnNameDelegate* delegate = static_cast<ColumnNameDelegate*>(
            ui->settings->itemDelegateForColumn(column));
        auto names = delegate->get_options_list();
        int var_index = names.indexOf(name) - 1;
        auto ind = ui->settings->model()->index(0, var_index);
        update_data(ind, ind, QList<int>({Qt::EditRole}));
        break;
      }
      case TwoAxesSettingsModel::Column::Error_Scatter:
      case TwoAxesSettingsModel::Column::Is_Active: {
        graph->setVisible(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Is_Active)
                ->data(Qt::DisplayRole)
                .value<bool>());
        bars_list[row]->setVisible(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Is_Active)
                ->data(Qt::DisplayRole)
                .value<bool>() &&
            ui->settings->item(row, TwoAxesSettingsModel::Column::Error_Scatter)
                ->data(Qt::DisplayRole)
                .value<bool>());
        break;
      }
      case TwoAxesSettingsModel::Column::Style: {
        auto cell_data = cell->data(Qt::DisplayRole).value<QString>();
        graph->setLineStyle(line_style_map[cell_data]);
        break;
      }
      case TwoAxesSettingsModel::Column::Line_Size:
      case TwoAxesSettingsModel::Column::Color: {
        QPen pen = QPen(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Color)
                ->background(),
            ui->settings->item(row, TwoAxesSettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>());

        graph->setPen(pen);
        bars_list[row]->setPen(pen);
        if (column == TwoAxesSettingsModel::Column::Line_Size) {
          break;
        }
      }
      case TwoAxesSettingsModel::Column::Scatter_Size:
      case TwoAxesSettingsModel::Column::Scatter: {
        auto shape =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter)
                ->data(Qt::DisplayRole);
        auto size =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter_Size)
                ->data(Qt::DisplayRole);

        graph->setScatterStyle(QCPScatterStyle(
            scatter_style_map[shape.value<QString>()],
            ui->settings->item(row, TwoAxesSettingsModel::Column::Color)
                ->background()
                .color(),
            size.value<double>()));
        break;
      }
    }
    ui->plot->replot();
  }

  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>()) {
    int start = std::min(bottomRight.column(), topLeft.column());
    int end = std::max(bottomRight.column(), topLeft.column());
    auto& manager = Manager::get_manager();

    if (bottomRight.row() > default_numbering_vector.size() - 1) {
      int last = default_numbering_vector.size();
      for (int i = last; i < bottomRight.row() + 1; ++i) {
        default_numbering_vector.append(i + 1);
      }
      auto index = ui->settings->model()->index(-2, -1);
      update_data(index, index);
    }
    for (int i = start; i < end + 1; ++i) {
      QSet<int> indexes;
      for (int ind_x : variable_to_graph_connection[i + 1].first) {
        indexes.insert(ind_x);
      }
      for (int ind_y : variable_to_graph_connection[i + 1].second) {
        indexes.insert(ind_y);
      }
      for (int graph_ind : indexes) {
        QString name_x =
            ui->settings->item(graph_ind, TwoAxesSettingsModel::Column::Axis_X)
                ->data(Qt::DisplayRole)
                .value<QString>();
        QString name_y =
            ui->settings->item(graph_ind, TwoAxesSettingsModel::Column::Axis_Y)
                ->data(Qt::DisplayRole)
                .value<QString>();
        ColumnNameDelegate* delegate = static_cast<ColumnNameDelegate*>(
            ui->settings->itemDelegateForColumn(
                TwoAxesSettingsModel::Column::Axis_X));
        int var_x_index = delegate->get_options_list().indexOf(name_x);
        int var_y_index = delegate->get_options_list().indexOf(name_y);
        QVector<double> x;
        QVector<double> y;
        QList<double> x_err;
        QList<double> y_err;
        if (var_x_index == 0) {
          x = default_numbering_vector;
          bars_list[graph_ind]->y->setVisible(false);
        } else {
          bars_list[graph_ind]->y->setVisible(true);
          x = QVector<double>::fromList(
              manager.variables[var_x_index - 1].measurements);
          y_err = manager.variables[var_x_index - 1].getErrors();
        }
        if (var_y_index == 0) {
          y = default_numbering_vector;
          bars_list[graph_ind]->x->setVisible(false);
        } else {
          bars_list[graph_ind]->x->setVisible(true);
          y = QVector<double>::fromList(
              manager.variables[var_y_index - 1].measurements);
          x_err = manager.variables[var_y_index - 1].getErrors();
        }
        bars_list[graph_ind]->y->setData(y_err, y_err);
        bars_list[graph_ind]->x->setData(x_err, x_err);
        ui->plot->graph(graph_ind)->setData(x, y);
      }
    }
    ui->plot->replot();
  }

  void update_var_names(const QModelIndex& topLeft,
                        const QModelIndex& bottomRight,
                        const QList<int>& roles = QList<int>()) {
    QList<QString> new_names;
    auto manager_variables = Manager::get_manager().variables;
    auto delegate =
        static_cast<ColumnNameDelegate*>(ui->settings->itemDelegateForColumn(
            TwoAxesSettingsModel::Column::Axis_X));
    for (int i = 1; i < manager_variables.size() + 1; ++i) {
      if (i < delegate->get_options_list().size()) {
        delegate->get_options_list()[i] = manager_variables[i - 1].short_name;
        continue;
      }
      delegate->get_options_list().append(manager_variables[i - 1].short_name);
    }
  }
};

class SortedLinePlot : public AbstractPlot {
 private:
  Ui::SortedLinePlot* ui;
  QMap<int, QPair<QList<int>, QList<int>>> variable_to_graph_connection;
  QVector<double> default_numbering_vector;
  QList<XYErrorBars*> bars_list;
  QList<QCPCurve*> curves;
  QMap<QString, QCPCurve::LineStyle> line_style_map{{"None", QCPCurve::lsNone},
                                                    {"Line", QCPCurve::lsLine}};

 public:
  SortedLinePlot(int graph_num = 1, QWidget* parent = nullptr)
      : ui(new Ui::SortedLinePlot) {
    ui->setupUi(this);

    ui->plot->xAxis->setLabel("Axis X");
    ui->plot->yAxis->setLabel("Axis Y");
    ui->settings->setRowCount(graph_num);
    int rows_count = ui->settings->rowCount();

    for (int i = 0; i < Manager::get_manager().variables[0].measurements.size();
         ++i) {
      default_numbering_vector.append(i + 1);
    }

    connect(ui->settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto manager_variables = Manager::get_manager().variables;
    auto manager_line_x = manager_variables[0];
    auto manager_line_y = manager_variables[0];
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_x = manager_line_x.measurements[0];
    double max_x = manager_line_x.measurements[0];
    double min_y = manager_line_y.measurements[0];
    double max_y = manager_line_y.measurements[0];

    variable_to_graph_connection[0] =
        QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
    for (int i = 0; i < graph_num; ++i) {
      variable_to_graph_connection[0].first.append(i);
      variable_to_graph_connection[0].second.append(i);
    }
    for (int i = 0; i < manager_variables.size(); ++i) {
      variable_to_graph_connection[i + 1] =
          QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
      static_cast<ColumnNameDelegate*>(
          ui->settings->itemDelegateForColumn(
              TwoAxesSettingsModel::Column::Axis_X))
          ->get_options_list()
          .append(manager_variables[i].short_name);
    }

    for (int i = 0; i < graph_num; ++i) {
      is_active = ui->settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = create_new_graph();

      if (is_active) {
        min_x = std::min(min_x, manager_line_x.getMinMeasurement());
        max_x = std::max(max_x, manager_line_x.getMaxMeasurement());
        min_y = std::min(min_y, manager_line_y.getMinMeasurement());
        max_y = std::max(max_y, manager_line_y.getMaxMeasurement());
      }
    }

    update_data(ui->settings->model()->index(-1, -1),
                ui->settings->model()->index(-1, -1));
    for (int i = 0; i < graph_num; ++i) {
      for (int k : QList({TwoAxesSettingsModel::Column::Is_Active,
                          TwoAxesSettingsModel::Column::Axis_X,
                          TwoAxesSettingsModel::Column::Style,
                          TwoAxesSettingsModel::Column::Scatter_Size,
                          TwoAxesSettingsModel::Column::Line_Size})) {
        redraw_settings(i, k);
      }
    }
    ui->plot->xAxis->setRange(min_x - (max_x - min_x) / 20.,
                              max_x + (max_x - min_x) / 20.);
    ui->plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                              max_y + (max_y - min_y) / 20.);

    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->replot();
  }
  ~SortedLinePlot() { delete ui; }

  QCPGraph* create_new_graph() {
    auto graph = ui->plot->addGraph();
    QCPCurve* curve = new QCPCurve(ui->plot->xAxis, ui->plot->yAxis);
    QCPErrorBars* errorBars_x =
        new QCPErrorBars(ui->plot->xAxis, ui->plot->yAxis);
    QCPErrorBars* errorBars_y =
        new QCPErrorBars(ui->plot->yAxis, ui->plot->xAxis);
    errorBars_y->setDataPlottable(curve);
    errorBars_x->setDataPlottable(curve);
    bars_list.append(new XYErrorBars(errorBars_x, errorBars_y));
    curves.append(curve);
    return graph;
  }

  int get_name_index(QString& name) {
    if (name == "None") {
      return -1;
    }
    for (int i = 0; i < Manager::get_manager().variables.size(); ++i) {
      if (name == Manager::get_manager().variables[i].short_name) {
        return i;
      }
    }
    return -2;
  }

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = curves[row];

    switch (column) {
      case TwoAxesSettingsModel::Column::Axis_X:
      case TwoAxesSettingsModel::Column::Axis_Y: {
        auto& man = Manager::get_manager();

        QString name_x =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_X)
                ->data(Qt::DisplayRole)
                .value<QString>();
        QString name_y =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_Y)
                ->data(Qt::DisplayRole)
                .value<QString>();
        auto name = ui->settings->item(row, column)
                        ->data(Qt::DisplayRole)
                        .value<QString>();
        int name_x_ind = get_name_index(name_x);
        int name_y_ind = get_name_index(name_y);
        int name_ind = get_name_index(name);

        int ind_remove_x;
        int ind_remove_y;
        for (auto& elems : variable_to_graph_connection) {
          ind_remove_x = elems.first.indexOf(row);
          ind_remove_y = elems.second.indexOf(row);
          if (ind_remove_x != -1) {
            elems.first.removeAt(ind_remove_x);
          }
          if (ind_remove_y != -1) {
            elems.second.removeAt(ind_remove_y);
          }
        }
        int x = get_name_index(name_x) + 1;
        int y = get_name_index(name_y) + 1;
        variable_to_graph_connection[x].first.append(row);
        variable_to_graph_connection[y].second.append(row);

        ColumnNameDelegate* delegate = static_cast<ColumnNameDelegate*>(
            ui->settings->itemDelegateForColumn(column));
        auto names = delegate->get_options_list();
        int var_index = names.indexOf(name) - 1;
        auto ind = ui->settings->model()->index(0, var_index);
        update_data(ind, ind, QList<int>({Qt::EditRole}));
        break;
      }
      case TwoAxesSettingsModel::Column::Error_Scatter:
      case TwoAxesSettingsModel::Column::Is_Active: {
        graph->setVisible(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Is_Active)
                ->data(Qt::DisplayRole)
                .value<bool>());
        bars_list[row]->setVisible(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Is_Active)
                ->data(Qt::DisplayRole)
                .value<bool>() &&
            ui->settings->item(row, TwoAxesSettingsModel::Column::Error_Scatter)
                ->data(Qt::DisplayRole)
                .value<bool>());
        break;
      }
      case TwoAxesSettingsModel::Column::Style: {
        auto cell_data = cell->data(Qt::DisplayRole).value<QString>();
        graph->setLineStyle(line_style_map[cell_data]);
        break;
      }
      case TwoAxesSettingsModel::Column::Line_Size:
      case TwoAxesSettingsModel::Column::Color: {
        QPen pen = QPen(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Color)
                ->background(),
            ui->settings->item(row, TwoAxesSettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>());

        graph->setPen(pen);
        bars_list[row]->setPen(pen);
        if (column == TwoAxesSettingsModel::Column::Line_Size) {
          break;
        }
      }
      case TwoAxesSettingsModel::Column::Scatter_Size:
      case TwoAxesSettingsModel::Column::Scatter: {
        auto shape =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter)
                ->data(Qt::DisplayRole);
        auto size =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter_Size)
                ->data(Qt::DisplayRole);

        graph->setScatterStyle(QCPScatterStyle(
            scatter_style_map[shape.value<QString>()],
            ui->settings->item(row, TwoAxesSettingsModel::Column::Color)
                ->background()
                .color(),
            size.value<double>()));
        break;
      }
    }
    ui->plot->replot();
  }

  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>()) {
    int start = std::min(bottomRight.column(), topLeft.column());
    int end = std::max(bottomRight.column(), topLeft.column());
    auto& manager = Manager::get_manager();

    if (bottomRight.row() > default_numbering_vector.size() - 1) {
      int last = default_numbering_vector.size();
      for (int i = last; i < bottomRight.row() + 1; ++i) {
        default_numbering_vector.append(i + 1);
      }
      auto index = ui->settings->model()->index(-2, -1);
      update_data(index, index);
    }
    for (int i = start; i < end + 1; ++i) {
      QSet<int> indexes;
      for (int ind_x : variable_to_graph_connection[i + 1].first) {
        indexes.insert(ind_x);
      }
      for (int ind_y : variable_to_graph_connection[i + 1].second) {
        indexes.insert(ind_y);
      }
      for (int graph_ind : indexes) {
        QString name_x =
            ui->settings->item(graph_ind, TwoAxesSettingsModel::Column::Axis_X)
                ->data(Qt::DisplayRole)
                .value<QString>();
        QString name_y =
            ui->settings->item(graph_ind, TwoAxesSettingsModel::Column::Axis_Y)
                ->data(Qt::DisplayRole)
                .value<QString>();
        ColumnNameDelegate* delegate = static_cast<ColumnNameDelegate*>(
            ui->settings->itemDelegateForColumn(
                TwoAxesSettingsModel::Column::Axis_X));
        int var_x_index = delegate->get_options_list().indexOf(name_x);
        int var_y_index = delegate->get_options_list().indexOf(name_y);
        QVector<double> x;
        QVector<double> y;
        QList<double> x_err;
        QList<double> y_err;
        if (var_x_index == 0) {
          x = default_numbering_vector;
          bars_list[graph_ind]->y->setVisible(false);
        } else {
          bars_list[graph_ind]->y->setVisible(true);
          x = QVector<double>::fromList(
              manager.variables[var_x_index - 1].measurements);
          y_err = manager.variables[var_x_index - 1].getErrors();
        }
        if (var_y_index == 0) {
          y = default_numbering_vector;
          bars_list[graph_ind]->x->setVisible(false);
        } else {
          bars_list[graph_ind]->x->setVisible(true);
          y = QVector<double>::fromList(
              manager.variables[var_y_index - 1].measurements);
          x_err = manager.variables[var_y_index - 1].getErrors();
        }
        QVector<QCPCurveData> curve_dat;
        for (int j = 0; j < x.size(); ++j) {
          curve_dat.append(QCPCurveData(j, x[j], y[j]));
        }
        bars_list[graph_ind]->y->setData(y_err, y_err);
        bars_list[graph_ind]->x->setData(x_err, x_err);
        // QCPCurve* curve = new QCPCurve(ui->plot->xAxis, ui->plot->yAxis);
        curves[graph_ind]->data()->set(curve_dat, true);
        // ui->plot->plottable->data()->set
        // ui->plot->graph(graph_ind)->setData(x, y);
      }
    }
    ui->plot->replot();
  }

  void update_var_names(const QModelIndex& topLeft,
                        const QModelIndex& bottomRight,
                        const QList<int>& roles = QList<int>()) {
    QList<QString> new_names;
    auto manager_variables = Manager::get_manager().variables;
    auto delegate =
        static_cast<ColumnNameDelegate*>(ui->settings->itemDelegateForColumn(
            TwoAxesSettingsModel::Column::Axis_X));
    for (int i = 1; i < manager_variables.size() + 1; ++i) {
      if (i < delegate->get_options_list().size()) {
        delegate->get_options_list()[i] = manager_variables[i - 1].short_name;
        continue;
      }
      delegate->get_options_list().append(manager_variables[i - 1].short_name);
    }
  }
};

#endif