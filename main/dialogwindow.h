#include <QDialog>
#include <QFile>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogWindow;
}
QT_END_NAMESPACE


class DialogWindow : public QDialog{
    Q_OBJECT
private:
public:
    Ui::DialogWindow *ui;
    DialogWindow(QWidget *parent = nullptr) : QDialog(parent), ui(new Ui::DialogWindow) {
        ui->setupUi(this);
        
        // QDialog(parent);
    }
    // virtual void accept() override;

    
};

/*
createGraph(){

DialogWindow dialog*;
dialog.exec()
data = dialig.data

switch(data)

class GraphTypeLINE

}

connect(button., createGraph);


class T:
private
public
public slots
*/