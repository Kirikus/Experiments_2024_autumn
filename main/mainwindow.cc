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

  manager.add_variable(VariableData({1, 3.5, -4}, err, "Test", "tst"));
  manager.add_variable(VariableData({4, 25, 26}, err2, "Test2", "tst2"));
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

  ui->graphics->addTab(new ScatterPlot(), "hmm?");

  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
}

MainWindow::~MainWindow() { delete ui; }
