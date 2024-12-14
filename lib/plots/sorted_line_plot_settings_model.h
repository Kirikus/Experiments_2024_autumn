#ifndef __SORTED_LINE_PLOT_SETTINGS_MODEL_H__
#define __SORTED_LINE_PLOT_SETTINGS_MODEL_H__

#include <QTableWidget>

class SortedLinePlotSettingsModel : public QTableWidget {
 public:
  enum Column {
    Is_Active = 0,  // Column for switching graph on/off
    Axis_X,         // Column for axis x values
    Axis_Y,         // Column for axis y values
    Style,          // Column for line type
    Color,          // Column for graph color
    Error_Scatter,  // Column for switching ErrorBars
    Scatter,        // Column for scatters type at graph
    Scatter_Size,   // Column for size of scatters
    Line_Size       // Column for line size
  };
  static QList<QString> heading;

  SortedLinePlotSettingsModel(QWidget* parent);

 private slots:
  void fillDefaultValues(const QModelIndex& parent, int start, int end);
};

#endif