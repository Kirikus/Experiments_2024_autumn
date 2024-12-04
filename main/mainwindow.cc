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

void MainWindow::changeTheme() {
  theme_is_dark = !theme_is_dark;
  if (theme_is_dark) {
    qApp->setPalette(*dark_palette);
    ui->action_Theme_button->setText("Light theme");
    return;
  }
  qApp->setPalette(style()->standardPalette());
  ui->action_Theme_button->setText("Dark theme");
}

void MainWindow::add_row() {
  Manager::get_manager().add_measurement_row();
  ui->tableData->model()->insertRows(0, 1);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  dark_palette = new QPalette();
  dark_palette->setColor(QPalette::Window, QColor(53, 53, 53));
  dark_palette->setColor(QPalette::WindowText, Qt::white);
  dark_palette->setColor(QPalette::Base, QColor(25, 25, 25));
  dark_palette->setColor(QPalette::AlternateBase, QColor(53, 53, 53));
  dark_palette->setColor(QPalette::ToolTipBase, Qt::white);
  dark_palette->setColor(QPalette::ToolTipText, Qt::white);
  dark_palette->setColor(QPalette::Text, Qt::white);
  dark_palette->setColor(QPalette::Button, QColor(53, 53, 53));
  dark_palette->setColor(QPalette::ButtonText, Qt::white);
  dark_palette->setColor(QPalette::BrightText, Qt::red);
  dark_palette->setColor(QPalette::Link, QColor(42, 130, 218));
  dark_palette->setColor(QPalette::Highlight, QColor(255, 99, 71));
  dark_palette->setColor(QPalette::HighlightedText, Qt::black);

  Manager &manager = Manager().get_manager();
  MeasurementModel *model_measurements = new MeasurementModel;
  ErrorModel *model_err = new ErrorModel;
  TitleModel *model_titles = new TitleModel;
  ErrorData *err = new ErrorAbsolute(0.);
  ErrorData *err2 = new ErrorRelative(0.12);

  manager.add_variable(VariableData(
      {2.69935,  2.9243,   3.162774, 3.41445,  3.6815,   3.961895, 4.2562,
       4.5663,   4.89055,  5.2293,   5.5812,   5.94897,  6.3296,   6.72345,
       7.13071,  7.5509,   7.9807,   8.42268,  8.8742,   9.3352,   9.80356,
       10.2713,  10.7554,  11.2442,  11.7301,  12.2289,  12.70928, 13.1904,
       13.671,   14.1545,  14.6217,  15.0894,  15.53537, 15.9716,  16.3996,
       16.8013,  17.185,   17.5534,  17.90682, 18.2352,  18.5353,  18.80472,
       19.0596,  19.2706,  19.4602,  19.6225,  19.7447,  19.8453,  19.9188,
       19.9404,  19.94304, 19.9189,  19.8432,  19.74847, 19.6225,  19.4617,
       19.27006, 19.0592,  18.807,   18.5305,  18.235,   17.908,   17.55834,
       17.1843,  16.8025,  16.39593, 15.9712,  15.5331,  15.0859,  14.6214,
       14.1542,  13.67707, 13.1903,  12.7024,  12.22087, 11.7394,  11.2412,
       10.75954, 10.2708,  9.8032,   9.33504,  8.8748,   8.4224,   7.980924,
       7.5509,   7.1307,   6.72374,  6.3292,   5.94845,  5.58196,  5.2297,
       4.8903,   4.56649,  4.2567,   3.96155,  3.681234, 3.4445,   3.16248,
       2.924323, 2.6993},
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
  ui->graphics->addTab(plot, "Start plot");

  connect(ui->buttonGraph, &QPushButton::clicked, this,
          &MainWindow::create_dialog);
  connect(ui->buttonAdd_row, &QPushButton::clicked, this, &MainWindow::add_row);
  connect(ui->actionOpen, &QAction::triggered, this,
          &MainWindow::choose_file_open);
  connect(ui->action_Theme_button, &QAction::triggered, this,
          &MainWindow::changeTheme);
  connect(model_err, &QAbstractTableModel::dataChanged, plot,
          &AbstractPlot::update_data);
  connect(model_measurements, &QAbstractTableModel::dataChanged, plot,
          &AbstractPlot::update_data);
  connect(ui->graphics, &QTabWidget::tabCloseRequested, &QTabWidget::removeTab);
}

MainWindow::~MainWindow() { delete ui; }