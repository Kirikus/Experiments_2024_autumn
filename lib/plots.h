#ifndef __PLOTS_H__
#define __PLOTS_H__

#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPen>
#include <QString>
#include <QWidget>

#include "delegates.h"
#include "graph_settings_model.h"
#include "manager.h"
#include "measurement_model.h"
#include "qcustomplot.h"

class AbstractPlot : public QWidget {
 public:
  QCustomPlot* plot;
  SettingsModel* settings;

  QMap<QString, QCPGraph::LineStyle> colors_map{
      {"Line", QCPGraph::lsLine},
      {"StepLeft", QCPGraph::lsStepLeft},
      {"StepRight", QCPGraph::lsStepRight},
      {"StepCenter", QCPGraph::lsStepCenter},
      {"Impulse", QCPGraph::lsImpulse}};
  QMap<QString, QCPScatterStyle::ScatterShape> scatters_map{
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

  virtual void update_data(const int topleft, const int bottomRight,
                           const QList<int>& roles = QList<int>()) = 0;

 public slots:
  virtual void update_data_slot(const QModelIndex& topleft,
                                const QModelIndex& bottomRight,
                                const QList<int>& roles = QList<int>()) = 0;
  virtual void redraw_settings(int row, int column) = 0;
};

class ScatterPlot : public AbstractPlot {
 public:
  ScatterPlot(QWidget* parent = nullptr) {
    plot = new QCustomPlot();
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    settings = new SettingsModel();
    int rows_count = settings->rowCount();

    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(Qt::Vertical, nullptr);
    layout->addWidget(splitter);
    splitter->addWidget(plot);
    splitter->addWidget(settings);
    connect(settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);

    auto manager_line = Manager::get_manager().variables;
    bool is_active;
    auto style = QCPGraph::lsLine;
    double min_y = manager_line[0].measurements[0];
    double max_y = manager_line[0].measurements[0];

    for (int i = 0; i < rows_count; ++i) {
      is_active = settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = plot->addGraph();

      update_data(i, i);

      if (is_active) {
        min_y = std::min(min_y,
                         *std::min_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));
        max_y = std::max(max_y,
                         *std::max_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));

        for (int k : QList({SettingsModel::Column::Is_Active,
                            SettingsModel::Column::Style,
                            SettingsModel::Column::Line_Size,
                            SettingsModel::Column::Scatter_Size})) {
          redraw_settings(i, k);
        }
      }
    }
    plot->xAxis->setRange(0, 1);
    plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                          max_y + (max_y - min_y) / 20.);

    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iRangeDrag, true);
    plot->replot();
  }

 public slots:
  virtual void redraw_settings(int row, int column) {
    auto cell = settings->item(row, column);
    auto graph = plot->graph(row);

    switch (column) {
      case SettingsModel::Column::Is_Active: {
        graph->setVisible(cell->data(Qt::DisplayRole).value<bool>());
        break;
      }
      case SettingsModel::Column::Style: {
        auto cell_data = cell->data(Qt::DisplayRole).value<QString>();
        graph->setLineStyle(colors_map[cell_data]);
        break;
      }
      case SettingsModel::Column::Line_Size:
      case SettingsModel::Column::Color: {
        graph->setPen(QPen(
            settings->item(row, SettingsModel::Column::Color)->background(),
            settings->item(row, SettingsModel::Column::Line_Size)
                ->data(Qt::DisplayRole)
                .value<double>()));
        break;
      }
      case SettingsModel::Column::Scatter_Size:
      case SettingsModel::Column::Scatter: {
        auto shape = settings->item(row, SettingsModel::Column::Scatter)
                         ->data(Qt::DisplayRole);
        auto size = settings->item(row, SettingsModel::Column::Scatter_Size)
                        ->data(Qt::DisplayRole);

        graph->setScatterStyle(
            QCPScatterStyle(scatters_map[shape.value<QString>()],
                            settings->item(row, SettingsModel::Column::Color)
                                ->background()
                                .color(),
                            size.value<double>()));
        break;
      }
    }
    plot->replot();
  }

  virtual void update_data_slot(
      const QModelIndex& topLeft, const QModelIndex& bottomRight,
      const QList<int>& roles = QList<int>()) override {
    update_data(topLeft.column(), bottomRight.column(), roles);
  }

 public:
  virtual void update_data(const int topLeft, const int bottomRight,
                           const QList<int>& roles = QList<int>()) override {
    int row_width = roles.size() / (topLeft - bottomRight + 1);
    bool line_changed;
    bool table_changed = false;
    for (int row = bottomRight; row < (topLeft + 1); ++row) {
      line_changed = false;
      for (int k = row_width * (row - bottomRight);
           k < row_width * (row - bottomRight + 1); ++k) {
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
      double step = .95 / manager_line.size();
      for (int i = 0; i < x.size(); ++i) {
        x[i] = step * i + 0.025;
      }
      plot->graph(row)->setData(x, y);
    }
    if (table_changed) plot->replot();
  }
};

#endif