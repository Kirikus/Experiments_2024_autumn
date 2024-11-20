#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogWindow;
}
QT_END_NAMESPACE


class DialogWindow : public QDialog{
    Q_OBJECT
private:
    QList<QString> graph_types = {"Line plot", "Scatter plot", "Histogram", "Heatmap"};
public:
    Ui::DialogWindow *ui;
    DialogWindow(QWidget *parent);

    //virtual void accept() override;

};