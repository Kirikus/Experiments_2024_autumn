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

 private slots:
  void add_row();
  void add_variable();
  void changeTheme();
  void create_dialog();
  void choose_file_open();
};
#endif  // MAINWINDOW_H
