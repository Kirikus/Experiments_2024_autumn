#include "dialogwindow.h"
#include "dialogamountrowswindow.h"
#include "../lib/plots/plots.h"
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
  switch (t) {
    case OneAxisPlotType: {
      auto* plot = new OneAxisPlot();
      int index = target_tab_widget->addTab(plot, new_name);
      target_tab_widget->setCurrentIndex(index);
      connect(err_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(measure_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      break;
    }
    case TwoAxesPlotType: {
      hide();
      int graphs_num;
      auto* dialog_ask_rows = new DialogAmountRowsWindow(graphs_num, nullptr);
      dialog_ask_rows->show();
      dialog_ask_rows->exec();
      if (graphs_num <= 0) {
        break;
      }

      auto* plot = new TwoAxesPlot(graphs_num);
      int index = target_tab_widget->addTab(plot, new_name);
      target_tab_widget->setCurrentIndex(index);
      connect(measure_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(err_model, &QAbstractTableModel::dataChanged, plot,
              &AbstractPlot::update_data);
      connect(titles_model, &QAbstractTableModel::dataChanged, plot,
              &TwoAxesPlot::update_var_names);
      break;
    }
    case HistogramType:
      break;
    case HeatmapType:
      break;
  }
  close();
}

DialogWindow::DialogWindow(QTabWidget* target_tab_widget,
                           MeasurementModel* meas_model, ErrorModel* err_model,
                           TitleModel* titles_model, QWidget* parent = nullptr)
    : QDialog(parent),
      ui(new Ui::DialogWindow),
      target_tab_widget(target_tab_widget),
      measure_model(meas_model),
      err_model(err_model),
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
