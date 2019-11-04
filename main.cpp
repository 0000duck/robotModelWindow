#include <QApplication>
#include <QWindow>
#include <QProcess>
#include <QTime>
#include "robotModel/robotModel.h"
#include "robotClient.h"

#include <iostream>

using namespace std;
using namespace rclib;

extern long PID;

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    cout << string(argv[1]) << " " << string(argv[2]) << " " << string(argv[3]) << " " << string(argv[4]) << endl;

    QString ip = argv[2];
    QString str_port(argv[3]);

    int port = str_port.toInt();

    while(1){
        bool state = RobotClient::initance()->initSystem(ip.toStdString().c_str(), port);
        if(state){
            RobotClient::initance()->sendKZXT(SWITCHOFF);
            break;
        }
    }

    PID = QString(argv[4]).toInt();

    WId windowWid = QString(argv[1]).toInt();

    QWindow *windowParent = QWindow::fromWinId(windowWid);

    RobotModel* robotModel=new RobotModel();
    robotModel->setProperty("_q_embedded_native_parent_handle",QVariant(windowWid));
    robotModel->winId();
    robotModel->windowHandle()->setParent(windowParent);

    QTime dieTime = QTime::currentTime().addMSecs(1000);  //加入延时防止显示错误
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);

    robotModel->show();

    return app.exec();
}
