#include "mainwindow.h"

#include <QDataStream>
#include <QFileDialog>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "../lib/error_model.h"
#include "../lib/graph_model.h"
#include "../lib/measurement_model.h"
#include "./ui_mainwindow.h"
#include "qcustomplot.h"
// #include "graph_model.h"

void MainWindow::choose_file_open() {
  QFile file(QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv"), this);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  MeasurementModel *model_measurements = new MeasurementModel;
  ErrorModel *model_err = new ErrorModel;
  Manager &manager = Manager().get_manager();

  ErrorData *err = new ErrorAbsolute(0.);
  ErrorData *err2 = new ErrorRelative(0.12);

  manager.add_variable(VariableData({1, 2, 3}, err, "Test", "tst"));
  manager.add_variable(VariableData({4, 5, 6}, err2, "Test2", "tst2"));
  ui->tableData->setModel(model_measurements);
  ui->tableErrors->setModel(model_err);

  // tests

  QTableWidget *test = ui->test_table;
  test->setRowCount(2);
  test->setColumnCount(3);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      QTableWidgetItem *item =
          new QTableWidgetItem(QString::number(i + 1) + QString::number(j + 1));
      test->setItem(i, j, item);
    }
  }

  // generate some data:
  QVector<double> x(101), y(101);  // initialize with entries 0..100
  for (int i = 0; i < 101; ++i) {
    x[i] = i / 50.0 - 1;  // x goes from -1 to 1
    y[i] = x[i] * x[i];   // let's plot a quadratic function
  }

  // QCustomPlot *custom = new QCustomPlot(this);

  auto customPlot = ui->widgetGraph2;
  // create graph and assign data to it:
  customPlot->addGraph();
  customPlot->graph(0)->setData(x, y);
  // give the axes some labels:
  customPlot->xAxis->setLabel("x");
  customPlot->yAxis->setLabel("y");
  // set axes ranges, so we see all data:
  customPlot->xAxis->setRange(-1, 1);
  customPlot->yAxis->setRange(0, 1);
  customPlot->replot();

  // SettingsItemDelegate *delegate = new SettingsItemDelegate();
  // test->setItemDelegateForColumn(2, delegate);

  ui->WWW->setModel(new ScatterPlot());

  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
}

MainWindow::~MainWindow() { delete ui; }
