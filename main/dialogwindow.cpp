

#include "dialogwindow.h"

#include "ui_dialogwindow.h"

DialogWindow::DialogWindow(QWidget *parent = nullptr)
    : QDialog(parent), ui(new Ui::DialogWindow) {
  ui->setupUi(this);
  ui->comboBox->addItems(graph_types);
}
