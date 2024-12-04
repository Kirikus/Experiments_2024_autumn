#ifndef __PLOTS_H__
#define __PLOTS_H__

#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPen>
#include <QString>
#include <QWidget>
#include <limits>

#include "../data/manager.h"
#include "../data/measurement_model.h"
#include "./ui_one_axis_plot.h"
#include "./ui_two_axes_plot.h"
#include "one_axis_settings_model.h"
#include "two_axes_settings_model.h"
#include "qcustomplot.h"

class AbstractPlot : public QWidget {
 public:
  QMap<QString, QCPGraph::LineStyle> line_style_map{
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
}
QT_END_NAMESPACE

class OneAxisPlot : public AbstractPlot {
 private:
  Ui::OneAxisPlot* ui;

 public:
  OneAxisPlot(QWidget* parent = nullptr) : ui(new Ui::OneAxisPlot) {
    ui->setupUi(this);

    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");
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
      auto graph = ui->plot->addGraph();

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

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = ui->plot->graph(row);

    switch (column) {
      case OneAxisSettingsModel::Column::Is_Active: {
        graph->setVisible(cell->data(Qt::DisplayRole).value<bool>());
        break;
      }
      case OneAxisSettingsModel::Column::Style: {
        auto cell_data = cell->data(Qt::DisplayRole).value<QString>();
        graph->setLineStyle(line_style_map[cell_data]);
        break;
      }
      case OneAxisSettingsModel::Column::Line_Size:
      case OneAxisSettingsModel::Column::Color: {
        graph->setPen(QPen(
            ui->settings->item(row, OneAxisSettingsModel::Column::Color)->background(),
            ui->settings->item(row, OneAxisSettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>()));
        break;
      }
      case OneAxisSettingsModel::Column::Scatter_Size:
      case OneAxisSettingsModel::Column::Scatter: {
        auto shape = ui->settings->item(row, OneAxisSettingsModel::Column::Scatter)
                         ->data(Qt::DisplayRole);
        auto size = ui->settings->item(row, OneAxisSettingsModel::Column::Scatter_Size)
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
    int row_width =
        roles.size() / (topLeft.column() - bottomRight.column() + 1);
    bool line_changed;
    bool table_changed = false;
    for (int row = bottomRight.column(); row < (topLeft.column() + 1); ++row) {
      line_changed = false;
      for (int k = row_width * (row - bottomRight.column());
           k < row_width * (row - bottomRight.column() + 1); ++k) {
        if (roles[k] == Qt::EditRole) {
          line_changed = true;
          break;
        }
      }
      if (line_changed || (roles.size() == 0)) {
        table_changed = true;
      } else {
        continue;
      }

      auto manager_line = Manager::get_manager().variables[row];
      QVector<double> x(manager_line.size());
      QVector<double> y = QVector<double>::fromList(manager_line.measurements);
      for (int i = 0; i < x.size(); ++i) {
        x[i] = i + 1;
      }
      ui->plot->graph(row)->setData(x, y);
    }
    if (table_changed) ui->plot->replot();
  }
};

class TwoAxesPlot : public AbstractPlot {
 private:
  Ui::TwoAxesPlot* ui;

 public:
  TwoAxesPlot(QWidget* parent = nullptr) : ui(new Ui::TwoAxesPlot) {
    ui->setupUi(this);

    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");
    int rows_count = ui->settings->rowCount();

    connect(ui->settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto manager_line_x = Manager::get_manager().variables[0];                  //ПОМЕНЯТЬ HARDCODE!!!
    auto manager_line_y = Manager::get_manager().variables[1];                  //ПОМЕНЯТЬ HARDCODE!!!
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_x = manager_line_x.measurements[0];
    double max_x = manager_line_x.measurements[0];
    double min_y = manager_line_y.measurements[0];
    double max_y = manager_line_y.measurements[0];

    for (int i = 0; i < 1; ++i) {                                               //ПОМЕНЯТЬ HARDCODE!!!
      is_active = ui->settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = ui->plot->addGraph();

      update_data(ui->settings->model()->index(i, i),
                  ui->settings->model()->index(i, i));

      if (is_active) {
        min_x = std::min(min_x, manager_line_x.getMinMeasurement());
        max_x = std::max(max_x, manager_line_x.getMaxMeasurement());
        min_y = std::min(min_y, manager_line_y.getMinMeasurement());
        max_y = std::max(max_y, manager_line_y.getMaxMeasurement());

        for (int k : QList({TwoAxesSettingsModel::Column::Is_Active,
                            TwoAxesSettingsModel::Column::Axis_X,
                            TwoAxesSettingsModel::Column::Axis_Y,
                            TwoAxesSettingsModel::Column::Line_Size,
                            TwoAxesSettingsModel::Column::Style,
                            TwoAxesSettingsModel::Column::Line_Size,
                            TwoAxesSettingsModel::Column::Scatter_Size})) {
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

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = ui->settings->item(row, column);
    auto graph = ui->plot->graph(row);

    switch (column) {
      case TwoAxesSettingsModel::Column::Axis_X:
      case TwoAxesSettingsModel::Column::Axis_Y: {                                  //ПОМЕНЯТЬ HARDCODE!!!
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
        graph->setPen(QPen(
            ui->settings->item(row, TwoAxesSettingsModel::Column::Color)->background(),
            ui->settings->item(row, TwoAxesSettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>()));
        break;
      }
      case TwoAxesSettingsModel::Column::Scatter_Size:
      case TwoAxesSettingsModel::Column::Scatter: {
        auto shape = ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter)
                         ->data(Qt::DisplayRole);
        auto size = ui->settings->item(row, TwoAxesSettingsModel::Column::Scatter_Size)
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
    // int row_width =
    //     roles.size() / (topLeft.column() - bottomRight.column() + 1);
    // bool line_changed;
    // bool table_changed = false;
    // for (int row = bottomRight.column(); row < (topLeft.column() + 1); ++row) {
    //   line_changed = false;
    //   for (int k = row_width * (row - bottomRight.column());
    //        k < row_width * (row - bottomRight.column() + 1); ++k) {
    //     if (roles[k] == Qt::EditRole) {
    //       line_changed = true;
    //       break;
    //     }
    //   }
    //   if (line_changed || (roles.size() == 0)) {
    //     table_changed = true;
    //   } else {
    //     continue;
    //   }

    //   auto manager_line = Manager::get_manager().variables[row];
    //   QVector<double> x(manager_line.size());
    //   QVector<double> y = QVector<double>::fromList(manager_line.measurements);
    //   for (int i = 0; i < x.size(); ++i) {
    //     x[i] = i + 1;
    //   }
    //   ui->plot->graph(row)->setData(x, y);
    // }
    bool table_changed = true;
    QVector<double> x = QVector<double>::fromList(Manager::get_manager().variables[0].measurements);
    QVector<double> y = QVector<double>::fromList(Manager::get_manager().variables[1].measurements);
    ui->plot->graph(0)->setData(x, y);
    if (table_changed) ui->plot->replot();
  }
};


#endif