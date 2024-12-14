#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QMap>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  Ui::MainWindow *ui;
  QPalette *dark_palette;
  bool theme_is_dark = false;
  QString active_file;

 private slots:
  void add_row();
  void add_variable();
  void changeTheme();

  // ask for plot type and create new tab with it
  void create_tab_with_plot_choice();

  // replace actual data with chosen file
  void choose_file_open();
  void import_data();
};
#endif  // MAINWINDOW_H
