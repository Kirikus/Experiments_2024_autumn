#include "dialogwindow.h"

#include "../lib/plots/plots.h"
#include "dialogamountrowswindow.h"
#include "ui_dialogwindow.h"

void DialogWindow::close_dialog() { close(); }

void DialogWindow::create_graph() {
  if (!target_tab_widget) {
    return;
  }
  QString chosen_type =
      ui->comboBox->currentData(Qt::DisplayRole).value<QString>();
  int t = plot_type_map[chosen_type];
  QString new_name =
      chosen_type + "_" + QString::number(target_tab_widget->count());
  int index;
  switch (t) {
    case UnsortedLinePlotType: {
      hide();
      int graphs_num;
      auto* dialog_ask_rows = new DialogAmountRowsWindow(graphs_num, nullptr);
      dialog_ask_rows->show();
      dialog_ask_rows->exec();
      if (graphs_num <= 0) {
        break;
      }

      auto* plot = new UnsortedLinePlot(graphs_num);
      index = target_tab_widget->addTab(plot, new_name);
      connect(measurement_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(error_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(titles_model, &QAbstractTableModel::dataChanged, plot,
              &UnsortedLinePlot::update_var_names);
      break;
    }
    case SortedLinePlotType: {
      hide();
      int graphs_num;
      auto* dialog_ask_rows = new DialogAmountRowsWindow(graphs_num, nullptr);
      dialog_ask_rows->show();
      dialog_ask_rows->exec();
      if (graphs_num <= 0) {
        break;
      }

      auto* plot = new SortedLinePlot(graphs_num);
      index = target_tab_widget->addTab(plot, new_name);
      connect(measurement_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(error_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(titles_model, &QAbstractTableModel::dataChanged, plot,
              &SortedLinePlot::update_var_names);
      break;
    }
    case HistogramType: {
      break;
    }
    case Heatmap2dType: {
      auto* plot = new Heatmap2d();
      index = target_tab_widget->addTab(plot, new_name);
      connect(measurement_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(measurement_model, &QAbstractTableModel::dataChanged, plot,
              &Heatmap2d::update_var_names);
      break;
    }
  }
  target_tab_widget->setCurrentIndex(index);

  close();
}

DialogWindow::DialogWindow(QTabWidget* target_tab_widget,
                           MeasurementModel* measurement_model, ErrorModel* error_model,
                           TitleModel* titles_model, QWidget* parent = nullptr)
    : QDialog(parent),
      ui(new Ui::DialogWindow),
      target_tab_widget(target_tab_widget),
      measurement_model(measurement_model),
      error_model(error_model),
      titles_model(titles_model) {
  ui->setupUi(this);
  ui->comboBox->addItems(graph_types);

  auto buttonApply = ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Apply);
  auto buttonDiscard =
      ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Discard);
  connect(buttonApply, &QPushButton::clicked, this,
          &DialogWindow::create_graph);
  connect(buttonDiscard, &QPushButton::clicked, this,
          &DialogWindow::close_dialog);
}
