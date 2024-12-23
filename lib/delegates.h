#ifndef __DELEGATES_H__
#define __DELEGATES_H__

#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QObject>
#include <QPainter>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTableWidget>

// color menu
class ColorDelegate : public QStyledItemDelegate {
 public:
  ColorDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    drawBackground(painter, option, index);
    QStyledItemDelegate::paint(painter, option, index);
  }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    return new QColorDialog(parent);
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QColorDialog* color_dialog = static_cast<QColorDialog*>(editor);
    model->setData(index, color_dialog->selectedColor(), Qt::BackgroundRole);
  }

 protected:
  virtual void drawBackground(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const {
    painter->fillRect(option.rect, index.model()->data(index).value<QColor>());
  }
};

class CheckBoxDelegate : public QStyledItemDelegate {
 public:
  CheckBoxDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

  bool editorEvent(QEvent* event, QAbstractItemModel* model,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) {
    if (event->type() == QEvent::MouseButtonRelease) {
      bool value = index.data(Qt::DisplayRole).toBool();
      model->setData(index, !value, Qt::DisplayRole);
      return true;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }

  void paint(QPainter* painter, const QStyleOptionViewItem& option,
             const QModelIndex& index) const {
    QStyleOptionButton option_button;
    option_button.rect = option.rect;

    bool isChecked = index.data(Qt::DisplayRole).toBool();
    if (isChecked) {
      option_button.state |= QStyle::State_On;
    } else {
      option_button.state |= QStyle::State_Off;
    }

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &option_button,
                                       painter);
  }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    return new QCheckBox(parent);
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QCheckBox* checker = static_cast<QCheckBox*>(editor);
    model->setData(index, checker->isChecked(), Qt::BackgroundRole);
  }
};

class MinMaxSpinBox : public QStyledItemDelegate {
 public:
  int minimum;
  int maximum;

  MinMaxSpinBox(int minimum, int maximum, QObject* parent = 0)
      : QStyledItemDelegate(parent), minimum{minimum}, maximum{maximum} {}

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
  }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    auto editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(minimum);
    editor->setMaximum(maximum);

    // updating data with every change, not only after 'Enter' key
    QObject::connect(
        editor, &QSpinBox::valueChanged, parent,
        [this, editor, parent, index]() {
          setModelData(editor,
                       static_cast<QTableWidget*>(parent->parent())->model(),
                       index);
        });

    return editor;
  }

  void setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
  }

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
  }
};

// choice ComboBox with empty options list
class AbstractComboBoxDelegate : public QStyledItemDelegate {
 public:
  QList<QString> options;

  AbstractComboBoxDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
  }

  QList<QString>& get_options_list() { return options; }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    auto editor = new QComboBox(parent);
    for (auto& option_string : options) {
      editor->addItem(option_string);
    }

    // updating data with every change, not only after 'Enter' key
    QObject::connect(
        editor, &QComboBox::currentTextChanged, parent,
        [this, editor, parent, index]() {
          setModelData(editor,
                       static_cast<QTableWidget*>(parent->parent())->model(),
                       index);
        });
    editor->setCurrentText(index.model()->data(index).toString());
    return editor;
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QComboBox* set_editor = static_cast<QComboBox*>(editor);
    model->setData(index, set_editor->currentText(), Qt::EditRole);
  }
};

class ColumnNameDelegate : public AbstractComboBoxDelegate {
 public:
  ColumnNameDelegate(QObject* parent = 0) : AbstractComboBoxDelegate(parent) {
    options = {"None"};
  }
};

class LineStyleDelegate : public AbstractComboBoxDelegate {
 public:
  LineStyleDelegate(QObject* parent = 0) : AbstractComboBoxDelegate(parent) {
    options = {"None",      "Line",       "StepLeft",
               "StepRight", "StepCenter", "Impulse"};
  }
};

class ScatterStyleDelegate : public AbstractComboBoxDelegate {
 public:
  ScatterStyleDelegate(QObject* parent = 0) : AbstractComboBoxDelegate(parent) {
    options = {"None",        "Cross",
               "Plus",        "Circle",
               "Disc",        "Square",
               "Diamond",     "Star",
               "Triangle",    "TriangleInverted",
               "CrossSquare", "PlusSquare",
               "CrossCircle", "PlusCircle",
               "Peace"};
  }
};

class GradientStyleDelegate : public AbstractComboBoxDelegate {
 public:
  GradientStyleDelegate(QObject* parent = 0)
      : AbstractComboBoxDelegate(parent) {
    options = {"Grayscale", "Hot",       "Cold", "Night",
               "Candy",     "Geography", "Ion",  "Thermal",
               "Polar",     "Spectrum",  "Jet",  "Hues"};
  }
};

#endif
