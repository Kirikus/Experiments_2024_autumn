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
  active_file = QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv");
  QFile file(active_file);
  file.close();
}

void MainWindow::import_data() {
  QFile file(QFileDialog::getOpenFileName(0, "Импортировать", "", "*.csv"));
  if (!file.open(QIODevice::ReadOnly)) {
    file.close();
    return;
  }
  QTextStream in(&file);
  auto &man = Manager::get_manager();
  auto last_var_num = man.variables.size();
  QStringList titles(in.readLine().split(","));
  for (auto title : titles) {
    VariableData var_data({}, new ErrorAbsolute(0.), title, title);
    man.add_variable(var_data);
  }

  auto vert_size = man.variables[0].size();
  QList<double> temp_list;
  QList<double> zero_list = QList<double>();
  QStringList line;

  for (int i = 0; i < vert_size; ++i) {
    if (in.atEnd()) {
      man.add_measurement_row(last_var_num, -1, zero_list);
      continue;
    }
    line = in.readLine().split(",");
    temp_list.clear();
    for (auto elem : line) {
      temp_list.append(elem.toDouble());
    }
    man.add_measurement_row(last_var_num, -1, temp_list);
  }
  while (!in.atEnd()) {
    man.add_measurement_row(0, -1, zero_list);
    line = in.readLine().split(",");
    for (int i = 0; i < last_var_num; ++i) {
      man.variables[i].measurements[man.variables[0].size() - 1] = 0.;
    }
    for (int i = last_var_num; i < last_var_num + line.size(); ++i) {
      man.variables[i].measurements[man.variables[0].size() - 1] =
          line[i - last_var_num].toDouble();
    }
  }

  ui->tableData->model()->insertColumns(last_var_num, titles.size());
  ui->tableErrors->model()->insertColumns(last_var_num, titles.size());
  ui->tableTitles->model()->insertColumns(last_var_num, titles.size());

  ui->tableData->model()->insertRows(vert_size,
                                     man.variables[0].size() - vert_size);
  ui->tableErrors->model()->insertRows(vert_size,
                                       man.variables[0].size() - vert_size);
  file.close();
}

void MainWindow::create_dialog() {
  DialogWindow *d = new DialogWindow(
      ui->graphics, dynamic_cast<MeasurementModel *>(ui->tableData->model()),
      dynamic_cast<ErrorModel *>(ui->tableErrors->model()),
      dynamic_cast<TitleModel *>(ui->tableTitles->model()), nullptr);
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
  ui->tableData->model()->insertRows(
      Manager::get_manager().variables[0].measurements.size() - 1, 1);
  ui->tableErrors->model()->insertRows(
      Manager::get_manager().variables[0].measurements.size() - 1, 1);
}

void MainWindow::add_variable() {
  auto &man = Manager::get_manager();
  int vars_num = Manager::get_manager().variables.size();
  int size = Manager::get_manager().variables[0].size();
  bool ok;
  ErrorAbsolute *err = new ErrorAbsolute(0.);
  QString name = QInputDialog::getText(
      this, "Add column", "Variable name:", QLineEdit::Normal, "", &ok);
  if (!ok) {
    return;
  }
  man.add_variable(VariableData(QList<double>(size, 0.), err, name, name));
  ui->tableData->model()->insertColumns(vars_num, 1);
  ui->tableErrors->model()->insertColumns(vars_num, 1);
  ui->tableTitles->model()->insertColumns(vars_num, 1);
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
  manager.add_variable(VariableData(
      {0.324855,  0.568848,  0.930837,  1.437063,  2.109153,  2.961049,
       3.996548,  5.207791,  6.574879,  8.06665,   9.64248,   11.254887,
       12.85262,  14.383919, 15.799644, 17.056003, 18.116721, 18.954519,
       19.551889, 19.901195, 20.00417,  19.870951, 19.518761, 18.970387,
       18.252575, 17.39446,  16.426107, 15.377234, 14.276149, 13.14893,
       12.018836, 10.905946, 9.826983,  8.795326,  7.821135,  6.911592,
       6.071207,  5.302159,  4.604667,  3.977346,  3.417556,  2.921717,
       2.485595,  2.104544,  1.773712,  1.488213,  1.243255,  1.034242,
       0.856843,  0.707045,  0.581173,  0.475906,  0.388272,  0.315639,
       0.255696,  0.206431,  0.166102,  0.133218,  0.106504,  0.084883,
       0.067446,  0.053432,  0.042207,  0.033245,  0.026114,  0.020456,
       0.015981,  0.012453,  0.009678,  0.007503,  0.005803,  0.004477,
       0.003445,  0.002646,  0.002027,  0.001549,  0.001182,  0.000899,
       0.000683,  0.000517,  0.000391,  0.000295,  0.000222,  0.000167,
       0.000125,  9.4e-05,   7e-05,     5.2e-05,   3.9e-05,   2.9e-05,
       2.2e-05,   1.6e-05,   1.2e-05,   9e-06,     6e-06,     5e-06,
       3e-06,     3e-06,     2e-06,     1e-06},
      err2, "Gamma", "gamma"));

  ui->setupUi(this);
  Heatmap2d *heatmap2d = new Heatmap2d();
  UnsortedLinePlot *unsorted_line_plot = new UnsortedLinePlot(3);
  SortedLinePlot *sorted_line_plot = new SortedLinePlot(3);
  changeTheme();

  ui->tableData->setModel(model_measurements);
  ui->tableErrors->setModel(model_err);
  ui->tableTitles->setModel(model_titles);
  ui->graphics->addTab(heatmap2d, "Heatmap2d");
  ui->graphics->addTab(unsorted_line_plot, "UnsortedLinePlot");
  ui->graphics->addTab(sorted_line_plot, "SortedLinePlot");
  ui->tableTitles->verticalHeader()->hide();

  connect(ui->button_Graph, &QPushButton::clicked, this,
          &MainWindow::create_dialog);
  connect(ui->button_Add_row, &QPushButton::clicked, this,
          &MainWindow::add_row);
  connect(ui->button_Add_variable, &QPushButton::clicked, this,
          &MainWindow::add_variable);
  connect(ui->action_Open, &QAction::triggered, this,
          &MainWindow::choose_file_open);
  connect(ui->action_Import_data, &QAction::triggered, this,
          &MainWindow::import_data);
  connect(ui->action_Theme_button, &QAction::triggered, this,
          &MainWindow::changeTheme);
          
  connect(model_measurements, &QAbstractTableModel::dataChanged, heatmap2d,
          &AbstractPlot::update_data);
  connect(model_titles, &QAbstractTableModel::dataChanged, heatmap2d,
          &Heatmap2d::update_var_names);

  connect(model_err, &QAbstractTableModel::dataChanged, unsorted_line_plot,
          &AbstractPlot::update_data);
  connect(model_measurements, &QAbstractTableModel::dataChanged, unsorted_line_plot,
          &AbstractPlot::update_data);
  connect(model_titles, &QAbstractTableModel::dataChanged, unsorted_line_plot,
          &UnsortedLinePlot::update_var_names);

  connect(model_err, &QAbstractTableModel::dataChanged, sorted_line_plot,
          &AbstractPlot::update_data);
  connect(model_measurements, &QAbstractTableModel::dataChanged, sorted_line_plot,
          &AbstractPlot::update_data);
  connect(model_titles, &QAbstractTableModel::dataChanged, sorted_line_plot,
          &SortedLinePlot::update_var_names);
  connect(ui->graphics, &QTabWidget::tabCloseRequested, &QTabWidget::removeTab);
}

MainWindow::~MainWindow() { delete ui; }