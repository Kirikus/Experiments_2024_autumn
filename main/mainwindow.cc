#include "mainwindow.h"

#include <QDataStream>
#include <QFileDialog>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "../lib/error_model.h"
#include "../lib/titles_model.h"
#include "../lib/measurement_model.h"
#include "../lib/plots.h"
#include "./ui_mainwindow.h"
#include "qcustomplot.h"

void MainWindow::choose_file_open() {
  QFile file(QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv"), this);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  Manager &manager = Manager().get_manager();
  MeasurementModel *model_measurements = new MeasurementModel;
  ErrorModel *model_err = new ErrorModel;
  ErrorData *err = new ErrorAbsolute(0.);
  ErrorData *err2 = new ErrorRelative(0.12);

  manager.add_variable(VariableData({1, 3.5, -4}, err, "Test", "tst"));
  manager.add_variable(VariableData({4, 25, 26}, err2, "Test2", "tst2"));

  ui->setupUi(this);
  ScatterPlot *plot = new ScatterPlot();

  ui->tableData->setModel(model_measurements);
  ui->tableErrors->setModel(model_err);
  ui->graphics->addTab(plot, "Scatter");

  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
  connect(model_measurements, &QAbstractTableModel::dataChanged, plot,
          &AbstractPlot::update_data_slot);
}


MainWindow::~MainWindow() { delete ui; }
