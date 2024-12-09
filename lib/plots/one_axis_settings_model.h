#ifndef __ONE_AXIS_GRAPH_SETTINGS_MODEL_H__
#define __ONE_AXIS_GRAPH_SETTINGS_MODEL_H__

#include <QTableWidget>

class OneAxisSettingsModel : public QTableWidget {
 public:
  enum Column {
    Is_Active = 0,  // Column for switching graph on/off
    Style,          // Column for line type
    Color,          // Column for graph color
    Error_Scatter,  // Column for scatters, resized with error value
    Scatter,        // Column for scatters type at graph
    Scatter_Size,   // Column for size of scatters
    Line_Size       // Column for line size
  };
  static QList<QString> heading;

  OneAxisSettingsModel(QWidget* parent);

 public slots:
  void fillDefaultValues(const QModelIndex& parent, int start, int end);
};

#endif