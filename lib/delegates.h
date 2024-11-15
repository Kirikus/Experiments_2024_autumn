#ifndef __DELEGATES_H__
#define __DELEGATES_H__

#include <QColorDialog>
#include <QComboBox>
#include <QObject>
#include <QPainter>
#include <QStyledItemDelegate>

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

class AbstractComboBoxDelegate : public QStyledItemDelegate {
 public:
  QList<QString> options;

  AbstractComboBoxDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
  }

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                        const QModelIndex& index) const {
    auto editor = new QComboBox(parent);
    for (auto& option_string : options) {
      editor->addItem(option_string);
    }
    return editor;
  }

  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const {
    QComboBox* set_editor = static_cast<QComboBox*>(editor);
    model->setData(index, set_editor->currentText(), Qt::EditRole);
  }
};

class LineStyleDelegate : public AbstractComboBoxDelegate {
 public:
  LineStyleDelegate(QObject* parent = 0) : AbstractComboBoxDelegate(parent) {
    options = {"Line", "StepLeft", "StepRight", "StepCenter", "Impulse"};
  }
};

class ScatterStyleDelegate : public AbstractComboBoxDelegate {
  ScatterStyleDelegate(QObject* parent = 0) : AbstractComboBoxDelegate(parent) {
    options = {"Cross",       "Plus",       "Circle",
               "Disc",        "Square",     "Diamond",
               "Star",        "Triangle",   "TriangleInverted",
               "CrossSquare", "PlusSquare", "CrossCircle",
               "PlusCircle",  "Peace",      "Custom"};
  }
};

#endif
