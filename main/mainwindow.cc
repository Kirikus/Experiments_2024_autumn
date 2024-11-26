#include "mainwindow.h"

#include <QDataStream>
#include <QFileDialog>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "../lib/data/error_model.h"
#include "../lib/data/measurement_model.h"
#include "../lib/plots/plots.h"
#include "../lib/data/titles_model.h"
#include "./ui_mainwindow.h"
#include "dialogwindow.h"
#include "qcustomplot.h"

void MainWindow::choose_file_open() {
  QFile file(QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv"), this);
}

void MainWindow::create_dialog() {
  DialogWindow *d = new DialogWindow(nullptr, this->ui->graphics);
  // d.ui->setupUi(d);
  d->show();
  d->exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  Manager &manager = Manager().get_manager();
  MeasurementModel *model_measurements = new MeasurementModel;
  ErrorModel *model_err = new ErrorModel;
  TitleModel *model_titles = new TitleModel;
  ErrorData *err = new ErrorAbsolute(0.);
  ErrorData *err2 = new ErrorRelative(0.12);

  manager.add_variable(VariableData(
      {14,  12, -7,  26,  9,   -4,  -17, -17, -2,  -1,  17,  29,  14,  26,  16,
       -9,  -3, 16,  25,  29,  19,  -1,  -26, -30, 13,  -13, -2,  -8,  30,  2,
       -27, 28, -12, -14, 1,   -4,  -29, 17,  -15, -23, -11, 30,  -29, 24,  27,
       -16, -5, 23,  -20, 2,   -16, -28, 15,  3,   -29, -15, 8,   -7,  -17, -9,
       29,  22, -16, 27,  -4,  19,  10,  -3,  -6,  -11, -10, 11,  1,   17,  10,
       20,  -2, -20, -26, -2,  22,  5,   -19, -3,  -13, -29, -17, 15,  -16, -15,
       19,  9,  4,   24,  -22, -3,  15,  -26, -2,  -23},
      err, "Test", "tst"));
  manager.add_variable(VariableData(
      {-20, -27, -26, 21, 1,   -5,  10,  18,  -9,  -6,  5,   -12, 11,  -24, -2,
       22,  29,  26,  10, 21,  -15, 25,  -8,  -26, -21, 18,  4,   9,   24,  8,
       21,  -16, 10,  27, -10, -10, 30,  -25, 7,   -5,  -27, -26, 14,  -28, 0,
       -7,  -24, -22, -1, 25,  6,   -15, -12, 25,  24,  -15, 11,  -30, 11,  13,
       22,  -29, 24,  14, 4,   -10, 8,   15,  16,  -23, 22,  23,  -12, -15, 17,
       -19, -19, -11, 5,  16,  12,  -4,  -20, 29,  -28, 5,   26,  16,  -28, -25,
       10,  30,  -5,  25, 4,   -28, -20, -30, -2,  -21},
      err2, "Test2", "tst2"));

  ui->setupUi(this);
  OneAxisPlot *plot = new OneAxisPlot();
  TwoAxesPlot *plot2 = new TwoAxesPlot();

  ui->tableData->setModel(model_measurements);
  ui->tableErrors->setModel(model_err);
  ui->tableTitles->setModel(model_titles);
  ui->graphics->addTab(plot, "OneAxisPlot");
  ui->graphics->addTab(plot2, "TwoAxesPlot");
  ui->tableTitles->verticalHeader()->hide();

  connect(ui->buttonGraph, &QPushButton::clicked, this,
          &MainWindow::create_dialog);
  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
  connect(model_measurements, &QAbstractTableModel::dataChanged, plot,
          &AbstractPlot::update_data);
  connect(model_measurements, &QAbstractTableModel::dataChanged, plot2,
          &TwoAxesPlot::update_var_names);
  connect(ui->graphics, &QTabWidget::tabCloseRequested,
          &QTabWidget::removeTab);
}

MainWindow::~MainWindow() { delete ui; }