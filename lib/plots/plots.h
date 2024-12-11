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
#include "./ui_one_axis_plot.h"
#include "./ui_two_axes_plot.h"
#include "one_axis_settings_model.h"
#include "qcustomplot.h"
#include "two_axes_settings_model.h"

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

QT_BEGIN_NAMESPACE
namespace Ui {
class OneAxisPlot;
class TwoAxesPlot;
}  // namespace Ui
QT_END_NAMESPACE

class OneAxisPlot : public AbstractPlot {
 private:
  Ui::OneAxisPlot* ui;
  QList<QCPErrorBars*> bars_list;
  QList<bool> bars_visibility;

 public:
  OneAxisPlot(QWidget* parent = nullptr) : ui(new Ui::OneAxisPlot) {
    ui->setupUi(this);

    ui->plot->setBackground(Qt::transparent);

    ui->plot->xAxis->setLabel("Measurement number");
    ui->plot->yAxis->setLabel("Value");
    int rows_count = ui->settings->rowCount();

    connect(ui->settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto manager_line = Manager::get_manager().variables;
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_y = manager_line[0].measurements[0];
    double max_y = manager_line[0].measurements[0];

    for (int i = 0; i < rows_count; ++i) {
      is_active = ui->settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = create_new_graph();

      update_data(ui->settings->model()->index(i, i),
                  ui->settings->model()->index(i, i));

      if (is_active) {
        min_y = std::min(min_y, manager_line[i].getMinMeasurement());
        max_y = std::max(max_y, manager_line[i].getMaxMeasurement());

        for (int k : QList({OneAxisSettingsModel::Column::Is_Active,
                            OneAxisSettingsModel::Column::Style,
                            OneAxisSettingsModel::Column::Line_Size,
                            OneAxisSettingsModel::Column::Scatter_Size})) {
          redraw_settings(i, k);
        }
      }
    }
    ui->plot->xAxis->setRange(0, manager_line[0].size() + 1);
    ui->plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                              max_y + (max_y - min_y) / 20.);

    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->replot();
  }
  ~OneAxisPlot() { delete ui; }

  QCPGraph* create_new_graph() {
    auto graph = ui->plot->addGraph();
    QCPErrorBars* errorBars =
        new QCPErrorBars(ui->plot->xAxis, ui->plot->yAxis);
    errorBars->setDataPlottable(graph);
    bars_list.append(errorBars);
    bars_visibility.append(true);
    return graph;
  }

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = ui->plot->graph(row);

    switch (column) {
      case OneAxisSettingsModel::Column::Is_Active: {
        graph->setVisible(cell->data(Qt::DisplayRole).value<bool>());
        bars_list[row]->setVisible(cell->data(Qt::DisplayRole).value<bool>() &&
                                   bars_visibility[row]);
        break;
      }
      case OneAxisSettingsModel::Column::Style: {
        auto cell_data = cell->data(Qt::DisplayRole).value<QString>();
        graph->setLineStyle(line_style_map[cell_data]);
        break;
      }
      case OneAxisSettingsModel::Column::Error_Scatter: {
        bars_visibility[row] = !bars_visibility[row];
        bars_list[row]->setVisible(cell->data(Qt::DisplayRole).value<bool>() &&
                                   bars_visibility[row]);
        break;
      }
      case OneAxisSettingsModel::Column::Line_Size:
      case OneAxisSettingsModel::Column::Color: {
        QPen pen = QPen(
            ui->settings->item(row, OneAxisSettingsModel::Column::Color)
                ->background(),
            ui->settings->item(row, OneAxisSettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>());

        graph->setPen(pen);
        bars_list[row]->setPen(pen);
        if (column == OneAxisSettingsModel::Column::Line_Size) {
          break;
        }
      }
      case OneAxisSettingsModel::Column::Scatter_Size:
      case OneAxisSettingsModel::Column::Scatter: {
        auto shape =
            ui->settings->item(row, OneAxisSettingsModel::Column::Scatter)
                ->data(Qt::DisplayRole);
        auto size =
            ui->settings->item(row, OneAxisSettingsModel::Column::Scatter_Size)
                ->data(Qt::DisplayRole);

        graph->setScatterStyle(QCPScatterStyle(
            scatter_style_map[shape.value<QString>()],
            ui->settings->item(row, OneAxisSettingsModel::Column::Color)
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
    if (bottomRight.column() >= ui->plot->graphCount()) {
      disconnect(ui->settings, &QTableWidget::cellChanged, this,
                 &AbstractPlot::redraw_settings);
      int last_ind = ui->settings->rowCount() - 1;
      ui->settings->setRowCount(bottomRight.column() + 1);
      for (int i = ui->plot->graphCount(); i < bottomRight.column() + 1; ++i) {
        create_new_graph();
        for (int k : QList({OneAxisSettingsModel::Column::Is_Active,
                            OneAxisSettingsModel::Column::Style,
                            OneAxisSettingsModel::Column::Line_Size,
                            OneAxisSettingsModel::Column::Scatter_Size})) {
          redraw_settings(i, k);
        }
      }
      connect(ui->settings, &QTableWidget::cellChanged, this,
              &AbstractPlot::redraw_settings);
    }

    for (int row = topLeft.column(); row < bottomRight.column() + 1; ++row) {
      auto manager_line = Manager::get_manager().variables[row];
      QVector<double> x(manager_line.size());
      QVector<double> y = QVector<double>::fromList(manager_line.measurements);
      for (int i = 0; i < x.size(); ++i) {
        x[i] = i + 1;
      }
      ui->plot->graph(row)->setData(x, y);
      QList<double> errors = Manager::get_manager().variables[row].getErrors();
      bars_list[row]->setData(errors, errors);
    }
    ui->plot->replot();
  }
};

class TwoAxesPlot : public AbstractPlot {
 private:
  Ui::TwoAxesPlot* ui;
  QMap<QString, QPair<QList<int>, QList<int>>> var_to_graph_connection;
  QVector<double> none_var;

 public:
  TwoAxesPlot(int graph_num = 1, QWidget* parent = nullptr)
      : ui(new Ui::TwoAxesPlot) {
    ui->setupUi(this);

    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");
    int rows_count = ui->settings->rowCount();

    for (int i = 0; i < Manager::get_manager().variables[0].measurements.size();
         ++i) {
      none_var.append(i + 1);
    }

    connect(ui->settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto man_vars = Manager::get_manager().variables;
    auto manager_line_x = man_vars[0];
    auto manager_line_y = man_vars[0];
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_x = manager_line_x.measurements[0];
    double max_x = manager_line_x.measurements[0];
    double min_y = manager_line_y.measurements[0];
    double max_y = manager_line_y.measurements[0];

    var_to_graph_connection["None"] =
        QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
    for (int i = 0; i < graph_num; ++i) {
      var_to_graph_connection["None"].first.append(i);
      var_to_graph_connection["None"].second.append(i);
    }
    for (int i = 0; i < man_vars.size(); ++i) {
      var_to_graph_connection[man_vars[i].short_name] =
          QPair<QList<int>, QList<int>>(QList<int>(), QList<int>());
      static_cast<ColumnNameDelegate*>(
          ui->settings->itemDelegateForColumn(
              TwoAxesSettingsModel::Column::Axis_X))
          ->options.append(man_vars[i].short_name);
    }

    for (int i = 0; i < graph_num; ++i) {
      is_active = ui->settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = ui->plot->addGraph();

      update_data(ui->settings->model()->index(0, i),
                  ui->settings->model()->index(0, i));

      if (is_active) {
        min_x = std::min(min_x, manager_line_x.getMinMeasurement());
        max_x = std::max(max_x, manager_line_x.getMaxMeasurement());
        min_y = std::min(min_y, manager_line_y.getMinMeasurement());
        max_y = std::max(max_y, manager_line_y.getMaxMeasurement());

        for (int k : QList({TwoAxesSettingsModel::Column::Is_Active,
                            TwoAxesSettingsModel::Column::Axis_X,
                            TwoAxesSettingsModel::Column::Axis_Y,
                            TwoAxesSettingsModel::Column::Style,
                            TwoAxesSettingsModel::Column::Scatter_Size,
                            TwoAxesSettingsModel::Column::Line_Size})) {
          redraw_settings(i, k);
        }
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
  ~TwoAxesPlot() { delete ui; }

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = ui->plot->graph(row);

    switch (column) {
      case TwoAxesSettingsModel::Column::Axis_X:
      case TwoAxesSettingsModel::Column::Axis_Y: {
        auto name_x =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_X)
                ->data(Qt::DisplayRole)
                .value<QString>();
        auto name_y =
            ui->settings->item(row, TwoAxesSettingsModel::Column::Axis_Y)
                ->data(Qt::DisplayRole)
                .value<QString>();
        auto name = ui->settings->item(row, column)
                        ->data(Qt::DisplayRole)
                        .value<QString>();

        int ind_remove_x;
        int ind_remove_y;
        for (auto& elems : var_to_graph_connection) {
          ind_remove_x = elems.first.indexOf(row);
          ind_remove_y = elems.second.indexOf(row);
          if (ind_remove_x != -1) {
            elems.first.removeAt(ind_remove_x);
          }
          if (ind_remove_y != -1) {
            elems.second.removeAt(ind_remove_y);
          }
        }
        var_to_graph_connection[name_x].first.append(row);
        var_to_graph_connection[name_y].second.append(row);

        ColumnNameDelegate* delegate = static_cast<ColumnNameDelegate*>(
            ui->settings->itemDelegateForColumn(column));
        auto names = delegate->options;
        int var_index = names.indexOf(name) - 1;
        auto ind = ui->settings->model()->index(0, var_index);
        update_data(ind, ind, QList<int>({Qt::EditRole}));
        break;
      }
      case TwoAxesSettingsModel::Column::Is_Active: {
        graph->setVisible(cell->data(Qt::DisplayRole).value<bool>());
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

    if (bottomRight.row() > none_var.size() - 1) {
      int last = none_var.size();
      for (int i = last; i < bottomRight.row() + 1; ++i) {
        none_var.append(i + 1);
      }
      auto ind = ui->settings->model()->index(-2, -1);
      update_data(ind, ind);
    }
    for (int i = start; i < end + 1; ++i) {
      QString name;
      if (i < 0) {
        name = "None";
      } else {
        name = Manager::get_manager().variables[i].short_name;
      }
      QSet<int> indexes;
      for (int ind_x : var_to_graph_connection[name].first) {
        indexes.insert(ind_x);
      }
      for (int ind_y : var_to_graph_connection[name].second) {
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
        int var_x_index = delegate->options.indexOf(name_x);
        int var_y_index = delegate->options.indexOf(name_y);
        QVector<double> x;
        QVector<double> y;
        if (var_x_index == 0) {
          x = none_var;
        } else {
          x = QVector<double>::fromList(
              Manager::get_manager().variables[var_x_index - 1].measurements);
        }
        if (var_y_index == 0) {
          y = none_var;
        } else {
          y = QVector<double>::fromList(
              Manager::get_manager().variables[var_y_index - 1].measurements);
        }
        ui->plot->graph(graph_ind)->setData(x, y);
      }
    }
    ui->plot->replot();
  }

  void update_var_names(const QModelIndex& topLeft,
                        const QModelIndex& bottomRight,
                        const QList<int>& roles = QList<int>()) {
    QList<QString> new_names;
    auto man_vars = Manager::get_manager().variables;
    auto delegate =
        static_cast<ColumnNameDelegate*>(ui->settings->itemDelegateForColumn(
            TwoAxesSettingsModel::Column::Axis_X));
    for (int i = 1; i < man_vars.size() + 1; ++i) {
      if (i < delegate->options.size()) {
        delegate->options[i] = man_vars[i - 1].short_name;
        continue;
      }
      delegate->options.append(man_vars[i - 1].short_name);
    }
  }
};

#endif