#include "mainwindow.h"

#include <QDataStream>
#include <QFileDialog>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTextStream>

#include "../lib/measurements.h"
#include "./ui_mainwindow.h"

void MainWindow::choose_file_open() {
  QFile file(QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv"), this);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);
  MeasurementModel *model = new MeasurementModel;
  MeasurementModel *model_err = new MeasurementModel;
  Manager *manager = Manager().get_manager();

  ErrorData *err = new ErrorAbsolute(0.);
  ErrorData *err2 = new ErrorAbsolute(0.12);
  
  manager->add_variable(
      VariableData({1, 2, 3}, {err2, err, err}, "Test", "tst"));
  manager->add_variable(
      VariableData({4, 5, 6}, {err, err2, err2}, "Test2", "tst2"));
  model->setManager(*manager);
  ui->TableData->setModel(model);

  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
}


MainWindow::~MainWindow() { delete ui; }
