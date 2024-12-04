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
      {14,  12, -7,  26,  9,   -4,  -17},
      err, "Test", "tst"));
  manager.add_variable(VariableData(
      {-30, -27, -26, 1,  3,    5,   10},
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
  connect(ui->graphics, &QTabWidget::tabCloseRequested,
          &QTabWidget::removeTab);
}

MainWindow::~MainWindow() { delete ui; }