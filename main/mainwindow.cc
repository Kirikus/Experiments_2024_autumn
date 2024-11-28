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
#include "../lib/data/titles_model.h"
#include "../lib/plots/plots.h"
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

  manager.add_variable(
      VariableData({2.6995483256594035, 2.924361919746503,  3.1627302599632174,
                    3.4149488862354445, 3.681266695207025,  3.9618805355175595,
                    4.25692981783946,   4.566491195552843,  4.890573375405005,
                    5.22911212043106,   5.581965509718882,  5.948909521203827,
                    6.329634004502086,  6.7237391107688245, 7.130732245641211,
                    7.550025609450739,  7.980934386031037,  8.422675637587558,
                    8.874367958231382,  9.33503193292388,   9.803591441763766,
                    10.278875841821113, 10.759623050151554, 11.24448354229212,
                    11.732025270549501, 12.220739495859789, 12.709047516057428,
                    13.195308262186504, 13.67782672317471,  14.15486314793475,
                    14.624642962929817, 15.085367332613094, 15.535224280099037,
                    15.972400276117616, 16.395092195905196, 16.80151953635193,
                    17.18993677959725,  17.558645784468034, 17.906008083795882,
                    18.230456963817552, 18.530509201619353, 18.804776337971372,
                    19.051975365923596, 19.270938720190006, 19.46062345858732,
                    19.620119534555325, 19.748657068972147, 19.8456125399724,
                    19.91051382113848,  19.943044011103904, 19.943044011103904,
                    19.91051382113848,  19.8456125399724,   19.748657068972147,
                    19.620119534555325, 19.460623458587317, 19.270938720190006,
                    19.051975365923592, 18.80477633797137,  18.53050920161935,
                    18.23045696381755,  17.90600808379588,  17.558645784468034,
                    17.189936779597243, 16.801519536351925, 16.395092195905193,
                    15.97240027611761,  15.535224280099031, 15.08536733261309,
                    14.624642962929814, 14.154863147934742, 13.677826723174707,
                    13.195308262186503, 12.709047516057424, 12.220739495859787,
                    11.732025270549494, 11.244483542292112, 10.759623050151554,
                    10.278875841821108, 9.803591441763762,  9.335031932923874,
                    8.874367958231378,  8.422675637587554,  7.9809343860310324,
                    7.550025609450739,  7.130732245641207,  6.723739110768824,
                    6.329634004502082,  5.9489095212038245, 5.581965509718876,
                    5.229112120431057,  4.890573375405003,  4.566491195552839,
                    4.256929817839457,  3.9618805355175555, 3.6812666952070234,
                    3.4149488862354445, 3.1627302599632148, 2.9243619197465023,
                    2.6995483256594035},
                   err, "Gauss", "Gs"));
  manager.add_variable(VariableData(
      {-20, -27, -26, 21, 1,   -5,  10,  18,  -9,  -6,  5,   -12, 11,  -24, -2,
       22,  29,  26,  10, 21,  -15, 25,  -8,  -26, -21, 18,  4,   9,   24,  8,
       21,  -16, 10,  27, -10, -10, 30,  -25, 7,   -5,  -27, -26, 14,  -28, 0,
       -7,  -24, -22, -1, 25,  6,   -15, -12, 25,  24,  -15, 11,  -30, 11,  13,
       22,  -29, 24,  14, 4,   -10, 8,   15,  16,  -23, 22,  23,  -12, -15, 17,
       -19, -19, -11, 5,  16,  12,  -4,  -20, 29,  -28, 5,   26,  16,  -28, -25,
       10,  30,  -5,  25, 4,   -28, -20, -30, -2,  -21},
      err2, "Random", "rnd"));

  ui->setupUi(this);
  LinePlot *plot = new LinePlot();

  ui->tableData->setModel(model_measurements);
  ui->tableErrors->setModel(model_err);
  ui->tableTitles->setModel(model_titles);
  ui->graphics->addTab(plot, "Scatter");

  connect(ui->buttonGraph, &QPushButton::clicked, this,
          &MainWindow::create_dialog);
  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
  connect(model_measurements, &QAbstractTableModel::dataChanged, plot,
          &AbstractPlot::update_data);
}

MainWindow::~MainWindow() { delete ui; }