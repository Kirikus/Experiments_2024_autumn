#include "dialogwindow.h"

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
  QString new_name =
      chosen_type + "_" + QString::number(target_tab_widget->count());
  switch (t) {
    case OneAxisPlotType: {
      int index = target_tab_widget->addTab(new OneAxisPlot(), new_name);
      target_tab_widget->setCurrentIndex(index);
      break;
    }
    case TwoAxesPlotType: {
      int index = target_tab_widget->addTab(new TwoAxesPlot(), new_name);
      target_tab_widget->setCurrentIndex(index);
      break;
    }
    case HistogramType:
      break;
    case HeatmapType:
      break;
  }
  close();
}

DialogWindow::DialogWindow(QWidget* parent = nullptr,
                           QTabWidget* target_tab_widget = nullptr)
    : QDialog(parent),
      ui(new Ui::DialogWindow),
      target_tab_widget(target_tab_widget) {
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
