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
  Manager *manager = Manager().get_manager();
  model->setManager(*manager);
  ui->tableView->setModel(model);

  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
}

MainWindow::~MainWindow() { delete ui; }
