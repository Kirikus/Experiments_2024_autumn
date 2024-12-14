#include "heatmap2d.h"

#include <QPen>

#include "../data/manager.h"
#include "../delegates.h"
#include "heatmap2d_settings_model.h"

Heatmap2d::Heatmap2d(int graph_num, QWidget* parent) : ui(new Ui::Heatmap2d) {
  ui->setupUi(this);
  color_map = new QCPColorMap(ui->plot->xAxis, ui->plot->yAxis);
  color_map->setGradient(QCPColorGradient::gpPolar);
  color_map->rescaleDataRange(true);
  color_map->setDataRange(QCPRange(0, 1));

  QCPColorScale* color_scale = new QCPColorScale(ui->plot);
  color_scale->setType(QCPAxis::atBottom);
  ui->plot->plotLayout()->addElement(1, 0, color_scale);
  color_map->setColorScale(color_scale);
  color_scale->axis()->setLabel("Data density");
  ui->plot->axisRect()->setupFullAxesBox(true);
  ui->plot->rescaleAxes();

  for (int i = 0; i < Manager::get_manager().variables.size(); ++i) {
    static_cast<ColumnNameDelegate*>(
        ui->settings->itemDelegateForColumn(
            Heatmap2dSettingsModel::Column::Axis_X))
        ->get_options_list()
        .append(Manager::get_manager().variables[i].short_name);
  }

  ui->plot->xAxis->setLabel("Axis X");
  ui->plot->yAxis->setLabel("Axis Y");
  ui->plot->setBackground(Qt::transparent);
  ui->settings->setRowCount(graph_num);
  default_distribution_vector = QVector<double>(
      Manager::get_manager().variables[0].measurements.size(), 1.);

  connect(ui->settings, &QTableWidget::cellChanged, this,
          &Heatmap2d::redraw_settings);

  auto manager_variables = Manager::get_manager().variables;
  auto manager_line_x = manager_variables[0];
  auto manager_line_y = manager_variables[0];
  bool is_active;
  double min_x = manager_line_x.measurements[0];
  double max_x = manager_line_x.measurements[0];
  double min_y = manager_line_y.measurements[0];
  double max_y = manager_line_y.measurements[0];

  for (int k : QList({Heatmap2dSettingsModel::Column::Axis_X,
                      Heatmap2dSettingsModel::Column::Axis_Y,
                      Heatmap2dSettingsModel::Column::Interpolate})) {
    redraw_settings(0, k);
  }
  ui->plot->replot();
}

int Heatmap2d::get_name_index(QString name) {
  if (name == "None") {
    return -1;
  }
  for (int i = 0; i < Manager::get_manager().variables.size(); ++i) {
    if (name == Manager::get_manager().variables[i].short_name) {
      return i;
    }
  }
  return -2;
}

void Heatmap2d::redraw_settings(int row, int column) {
  // int row: for QT connect compability, we know it is always 0

  auto cell = ui->settings->item(0, column);

  switch (column) {
    case Heatmap2dSettingsModel::Column::Axis_X:
    case Heatmap2dSettingsModel::Column::Axis_Y:
    case Heatmap2dSettingsModel::Column::Distribution: {
      QString name_x =
          ui->settings->item(0, Heatmap2dSettingsModel::Column::Axis_X)
              ->data(Qt::DisplayRole)
              .value<QString>();
      QString name_y =
          ui->settings->item(0, Heatmap2dSettingsModel::Column::Axis_Y)
              ->data(Qt::DisplayRole)
              .value<QString>();
      auto name = ui->settings->item(0, Heatmap2dSettingsModel::Column::Axis_X)
                      ->data(Qt::DisplayRole)
                      .value<QString>();

      ui->plot->xAxis->setLabel(name_x);
      ui->plot->yAxis->setLabel(name_y);

      ColumnNameDelegate* delegate =
          static_cast<ColumnNameDelegate*>(ui->settings->itemDelegateForColumn(
              Heatmap2dSettingsModel::Column::Axis_X));
      auto names = delegate->get_options_list();
      int var_index = names.indexOf(name) -
                      1;  // offset because in delegate name 'None' is the first
      auto ind = ui->settings->model()->index(0, var_index);
      update_data(ind, ind, QList<int>({Qt::EditRole}));
      break;
    }
    case Heatmap2dSettingsModel::Column::Gradient: {
      color_map->setGradient(gradient_style[ui->settings->item(0, column)
                                                ->data(Qt::DisplayRole)
                                                .value<QString>()]);
    }
    case Heatmap2dSettingsModel::Column::Interpolate: {
      bool interpolate_on =
          ui->settings->item(0, Heatmap2dSettingsModel::Column::Interpolate)
              ->data(Qt::DisplayRole)
              .value<bool>();
      color_map->setInterpolate(interpolate_on);
      break;
    }
  }
  ui->plot->replot();
}

void Heatmap2d::update_data(const QModelIndex& topLeft,
                            const QModelIndex& bottomRight,
                            const QList<int>& roles) {
  auto& manager = Manager::get_manager();
  int x_index = get_name_index(
      ui->settings->item(0, Heatmap2dSettingsModel::Column::Axis_X)
          ->data(Qt::DisplayRole)
          .value<QString>());
  int y_index = get_name_index(
      ui->settings->item(0, Heatmap2dSettingsModel::Column::Axis_Y)
          ->data(Qt::DisplayRole)
          .value<QString>());

  // block for processing insert rows events
  if (bottomRight.row() > default_distribution_vector.size() - 1) {
    for (int i = default_distribution_vector.size() - 1; i < bottomRight.row();
         ++i) {
      default_distribution_vector.push_back(1.);
    }
    auto index = ui->settings->model()->index(-1, -1);
    update_data(index, index);
  }

  int start = std::min(bottomRight.column(), topLeft.column());
  int end = std::max(bottomRight.column(), topLeft.column());

  // check that changed variables are linked with our data
  if (!((start <= x_index && x_index <= end) ||
        (start <= y_index && y_index <= end) || (x_index == -1) ||
        (y_index == -1))) {
    return;
  }

  double start_x_range;
  double start_y_range;
  double end_x_range;
  double end_y_range;
  int distribution =
      ui->settings->item(0, Heatmap2dSettingsModel::Column::Distribution)
          ->data(Qt::DisplayRole)
          .value<int>();

  // getting data boundaries
  if (x_index == -1) {
    start_x_range = 0.;
    end_x_range = default_distribution_vector.size() - 1;
  } else {
    start_x_range = manager.variables[x_index].getMinMeasurement();
    end_x_range = manager.variables[x_index].getMaxMeasurement();
  }
  if (y_index == -1) {
    start_y_range = 0.;
    end_y_range = default_distribution_vector.size() - 1;
  } else {
    start_y_range = manager.variables[y_index].getMinMeasurement();
    end_y_range = manager.variables[y_index].getMaxMeasurement();
  }

  QVector<double> x_distributions;
  QVector<double> y_distributions;
  if (x_index == -1) {
    x_distributions = default_distribution_vector;
  } else {
    x_distributions = manager.variables[x_index].get_distribution(distribution);
  }
  if (y_index == -1) {
    y_distributions = default_distribution_vector;
  } else {
    y_distributions = manager.variables[y_index].get_distribution(distribution);
  }

  // block for processing degenerate distribution
  int x_distribution, y_distribution;
  if (end_x_range - start_x_range == 0) {
    x_distributions = QVector<double>({0., 1., 0.});
    x_distribution = 3;
    start_x_range -= .5;
    end_x_range += .5;
  } else {
    x_distribution = distribution;
  }
  if (end_y_range - start_y_range == 0) {
    y_distributions = QVector<double>({0., 1., 0.});
    y_distribution = 3;
    start_y_range -= .5;
    end_y_range += .5;
  } else {
    y_distribution = distribution;
  }

  color_map->data()->setSize(x_distribution, y_distribution);
  for (int i = 0; i < x_distribution; ++i) {
    for (int j = 0; j < y_distribution; ++j) {
      color_map->data()->setCell(i, j, x_distributions[i] * y_distributions[j]);
    }
  }

  color_map->data()->setRange(QCPRange(start_x_range, end_x_range),
                              QCPRange(start_y_range, end_y_range));
  color_map->rescaleDataRange();
  ui->plot->rescaleAxes();
  ui->plot->replot();
}

void Heatmap2d::update_var_names(const QModelIndex& topLeft,
                                 const QModelIndex& bottomRight,
                                 const QList<int>& roles) {
  QList<QString> new_names;
  auto manager_variables = Manager::get_manager().variables;
  auto delegate =
      static_cast<ColumnNameDelegate*>(ui->settings->itemDelegateForColumn(
          Heatmap2dSettingsModel::Column::Axis_X));
  for (int i = 1; i < manager_variables.size() + 1; ++i) {
    if (i < delegate->get_options_list().size()) {
      delegate->get_options_list()[i] = manager_variables[i - 1].short_name;
      continue;
    }
    delegate->get_options_list().append(manager_variables[i - 1].short_name);
  }
}
