#include "mainwindow.h"

#include <QDataStream>
#include <QFileDialog>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTextStream>

#include "../lib/error_model.h"
#include "../lib/measurement_model.h"
#include "./ui_mainwindow.h"

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


  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
}



MainWindow::~MainWindow() { delete ui; }
