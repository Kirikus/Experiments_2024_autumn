#ifndef __TWO_AXES_GRAPH_SETTINGS_MODEL_H__
#define __TWO_AXES_GRAPH_SETTINGS_MODEL_H__

#include <QTableWidget>

class TwoAxesSettingsModel : public QTableWidget {
 public:
  enum Column {
    Is_Active = 0,  // Column for switching graph on/off
    Axis_X,         // Column for axis x values
    Axis_Y,         // Column for axis y values
    Style,          // Column for line type
    Color,          // Column for graph color
    Scatter,        // Column for scatters type at graph
    Scatter_Size,   // Column for size of scatters
    Line_Size       // Column for line size
  };
  static QList<QString> heading;

  TwoAxesSettingsModel(QWidget* parent);

 private slots:
  void fillDefaultValues(const QModelIndex& parent, int start, int end);
};

#endif