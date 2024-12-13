#ifndef __DIALOGAMOUNTROWSWINDOW_H__
#define __DIALOGAMOUNTROWSWINDOW_H__

#include <QPushButton>

#include "ui_dialogamountrowswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogAmountRowsWindow;
}
QT_END_NAMESPACE

class DialogAmountRowsWindow : public QDialog {
  Q_OBJECT
 public:
  Ui::DialogAmountRowsWindow* ui;
  int& rows_amount;
  DialogAmountRowsWindow(int& rows_amount, QWidget* parent)
      : rows_amount{rows_amount},
        QDialog(parent),
        ui(new Ui::DialogAmountRowsWindow) {
    ui->setupUi(this);
    auto buttonApply =
        ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Apply);
    auto buttonDiscard =
        ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Discard);
    connect(buttonApply, &QPushButton::clicked, this,
            &DialogAmountRowsWindow::apply);
    connect(buttonDiscard, &QPushButton::clicked, this,
            &DialogAmountRowsWindow::close_dialog);
  }

 private slots:
  void apply() {
    rows_amount = ui->spinBox->value();
    close();
  }

  void close_dialog() {
    rows_amount = -1;
    close();
  }
};

#endif