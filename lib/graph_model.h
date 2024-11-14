#ifndef __GRAPH_MODEL_H__
#define __GRAPH_MODEL_H__

#include <QHBoxLayout>
#include <QList>
#include <QObject>
#include <QPen>
#include <QString>
#include <QWidget>

#include "delegates.h"
#include "manager.h"
#include "measurement_model.h"
#include "qcustomplot.h"

class SettingsModel : public QTableWidget {
 public:
  SettingsModel(QWidget* parent = nullptr) : QTableWidget(parent) {
    setRowCount(Manager::get_manager().variables.size());
    setColumnCount(3);
    setItemDelegateForColumn(1, new LineStyleDelegate());
    setItemDelegateForColumn(2, new ColorDelegate());

    int rows_size = rowCount();
    for (int i = 0; i < rows_size; ++i) {
      QTableWidgetItem* item0 = new QTableWidgetItem("1");
      QTableWidgetItem* itemNone = new QTableWidgetItem("None");
      QTableWidgetItem* itemColor = new QTableWidgetItem("");

      setItem(i, 0, item0);
      setItem(i, 1, itemNone);
      setItem(i, 2, itemColor);
    }

    QTableWidgetItem* item0 = new QTableWidgetItem("Is Active");
    QTableWidgetItem* item1 = new QTableWidgetItem("Style");
    QTableWidgetItem* item2 = new QTableWidgetItem("Color");

    setHorizontalHeaderItem(0, item0);
    setHorizontalHeaderItem(1, item1);
    setHorizontalHeaderItem(2, item2);
  }
};

class AbstractPlot : public QWidget {
 public:
  QCustomPlot* plot;
  SettingsModel* settings;
  virtual void update_settings(int num) = 0;
  virtual void update_data(int num) = 0;
 public slots:
  virtual void draw(QModelIndex& index) = 0;

 protected:
  void create_environment(QWidget* parent = nullptr) {
    plot = new QCustomPlot();
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    int line_size = Manager::get_manager().variables.size();
    settings = new SettingsModel();
    for (int i = 0; i < line_size; ++i) {
      plot->addGraph();
      update_data(i);
      update_settings(i);
      QRgb rgb = qRgba(rand() % 255, rand() % 255, rand() % 255, 255);
      plot->graph(i)->setPen(QPen(QColor(rgb), 1));
    }

    QHBoxLayout* layout = new QHBoxLayout(parent);
    QSplitter* splitter = new QSplitter(Qt::Vertical, nullptr);
    layout->addWidget(splitter);
    splitter->addWidget(plot);
    splitter->addWidget(settings);
    // QObject::connect(settings, &QTableWidget::dataChanged, parent,
    //                  &AbstractPlot::draw);
  }
};

class ScatterPlot : public AbstractPlot {
 public:
  ScatterPlot(QWidget* parent = nullptr) { create_environment(this); }
 public slots:

  virtual void draw(QModelIndex& index) {}

 public:
  virtual void update_data(int num) override {
    auto manager_line = Manager::get_manager().variables[num];
    QVector<double> x(manager_line.size());
    QVector<double> y = QVector<double>::fromList(manager_line.measurements);
    double step = .95 / manager_line.size();
    for (int i = 0; i < x.size(); ++i) {
      x[i] = step * i + 0.025;
    }
    plot->graph(num)->setData(x, y);
    plot->replot();
  }

  virtual void update_settings(int num) override {
    auto manager_line = Manager::get_manager().variables;
    bool is_active =
        settings->item(num, 0)->data(Qt::DisplayRole).value<bool>();
    auto graph = plot->graph(num);
    if (is_active) {
      double min_y = manager_line[0].measurements[0];
      double max_y = manager_line[0].measurements[0];

      for (int i = 0; i < manager_line.size(); ++i) {
        min_y = std::min(min_y,
                         *std::min_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));
        max_y = std::max(max_y,
                         *std::max_element(manager_line[i].measurements.begin(),
                                           manager_line[i].measurements.end()));
      }

      plot->xAxis->setRange(0, 1);  // manager_line[0].size()
      plot->yAxis->setRange(min_y - (max_y - min_y) / 20.,
                            max_y + (max_y - min_y) / 20.);

      auto style = QCPGraph::lsLine;
      QString user_style =
          settings->item(num, 1)->data(Qt::DisplayRole).value<QString>();
      if (user_style == "StepLeft") style = QCPGraph::lsStepLeft;
      if (user_style == "StepCenter") style = QCPGraph::lsStepCenter;
      if (user_style == "StepRight") style = QCPGraph::lsStepRight;
      if (user_style == "Impulse") style = QCPGraph::lsImpulse;
      graph->setLineStyle(style);

      auto color = settings->item(num, 2)->background();
      graph->setPen(QPen(color, 1));
    }
    graph->setVisible(is_active);
    plot->replot();
  }
};

#endif