#ifndef __DIALOG_ASK_NUMBER_H__
#define __DIALOG_ASK_NUMBER_H__

#include <QPushButton>

#include "ui_dialog_ask_number.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogAskNumber;
}
QT_END_NAMESPACE

// additional dialog for setting rows number
class DialogAskNumber : public QDialog {
  Q_OBJECT
 public:
  Ui::DialogAskNumber* ui;
  int& output;

  DialogAskNumber(QString text, int& output, QWidget* parent = nullptr)
      : output{output}, QDialog(parent), ui(new Ui::DialogAskNumber) {
    ui->setupUi(this);
    ui->label->setText(text);

    auto buttonApply =
        ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Apply);
    auto buttonDiscard =
        ui->buttonBoxApplyDiscard->button(QDialogButtonBox::Discard);
    connect(buttonApply, &QPushButton::clicked, this, &DialogAskNumber::apply);
    connect(buttonDiscard, &QPushButton::clicked, this,
            &DialogAskNumber::close_dialog);
  }

  void setMinimum(int minimum_value) { ui->spinBox->setMinimum(minimum_value); }

  void setMaximum(int maximum_value) { ui->spinBox->setMaximum(maximum_value); }

 private slots:
  void apply() {
    output = ui->spinBox->value();
    close();
  }

  void close_dialog() { close(); }
};

#endif