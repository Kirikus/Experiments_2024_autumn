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

  virtual void set_start_pose() = 0;
  virtual void update_data(QList<int> indexes) = 0;
 public slots:
  virtual void redraw_settings(int row, int column) = 0;

 protected:
  void start_init(QWidget* parent = nullptr) {
    plot = new QCustomPlot();
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    settings = new SettingsModel();
    int rows_count = settings->rowCount();
    set_start_pose();

    for (int i = 0; i < rows_count; ++i) {
      update_data({i});
    }

    QHBoxLayout* layout = new QHBoxLayout(parent);
    QSplitter* splitter = new QSplitter(Qt::Vertical, nullptr);
    layout->addWidget(splitter);
    splitter->addWidget(plot);
    splitter->addWidget(settings);
    connect(settings, &QTableWidget::cellChanged, this,
            &AbstractPlot::redraw_settings);
  }
};

class ScatterPlot : public AbstractPlot {
 public:
  ScatterPlot(QWidget* parent = nullptr) { start_init(this); }
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
                            settings->item(row, 2)->background().color(),
                            size.value<double>()));
        break;
      }
    }
    plot->replot();
  }

 public:
  virtual void update_data(QList<int> indexes) override {
    if (indexes.size() != 1) {
      return;
    }
    auto manager_line = Manager::get_manager().variables[indexes[0]];
    QVector<double> x(manager_line.size());
    QVector<double> y = QVector<double>::fromList(manager_line.measurements);
    double step = .95 / manager_line.size();
    for (int i = 0; i < x.size(); ++i) {
      x[i] = step * i + 0.025;
    }
    plot->graph(indexes[0])->setData(x, y);
    plot->replot();
  }

  virtual void set_start_pose() override {
    auto manager_line = Manager::get_manager().variables;
    int rows_count = settings->rowCount();
    bool is_active = false;
    auto style = QCPGraph::lsLine;
    double min_y = manager_line[0].measurements[0];
    double max_y = manager_line[0].measurements[0];

    for (int i = 0; i < rows_count; ++i) {
      is_active = settings->item(i, 0)->data(Qt::DisplayRole).value<bool>();
      auto graph = plot->addGraph();
      if (is_active) {
        min_y = std::min(min_y,
                         *std::min_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));
        max_y = std::max(max_y,
                         *std::max_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));

        QString user_style = settings->item(i, SettingsModel::Column::Style)
                                 ->data(Qt::DisplayRole)
                                 .value<QString>();
        graph->setLineStyle(colors_map[user_style]);

        auto color =
            settings->item(i, SettingsModel::Column::Color)->background();
        graph->setPen(
            QPen(color, settings->item(i, SettingsModel::Column::Scatter_Size)
                            ->data(Qt::DisplayRole)
                            .value<double>()));
      }
      graph->setVisible(is_active);
    }
    plot->xAxis->setRange(0, 1);
    plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                          max_y + (max_y - min_y) / 20.);

    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iRangeDrag, true);
    plot->replot();
  }
};

#endif