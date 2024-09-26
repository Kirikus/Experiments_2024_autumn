#include "mainwindow.h"

#include "./ui_mainwindow.h"

void MainWindow::choose_file_open() {
  std::cout << 1 << std::endl;
  QFile file( QFileDialog::getOpenFileName(0, "Открыть", "", "*.csv"), this );
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);

  QPushButton* open_file_button = new QPushButton("Open file", this);
  QObject::connect(open_file_button, SIGNAL(clicked()), SLOT(choose_file_open()));

}

MainWindow::~MainWindow() { delete ui; }
