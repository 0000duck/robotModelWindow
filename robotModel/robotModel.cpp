#include "robotModel.h"
#include "robotClient.h"

using namespace rclib;

long PID;

RobotModel::RobotModel(QWidget *parent):QWidget (parent){
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setFixedSize(450,300);

    m_robotModelWidget = new robotModelWidget(this);
    QHBoxLayout* layoutRobotModelWidget = new QHBoxLayout(this);
    layoutRobotModelWidget->addWidget(m_robotModelWidget);
    layoutRobotModelWidget->setContentsMargins(0,0,0,0);

    buttonZoomUp = new QPushButton();
    buttonZoomDown = new QPushButton();
    buttonReturnModel = new QPushButton();
    buttonChangeDisplay = new QPushButton();

    buttonZoomUp->setStyleSheet("background-color: rgb(96,96,96)");
    buttonZoomDown->setStyleSheet("background-color: rgb(96,96,96)");
    buttonReturnModel->setStyleSheet("background-color: rgb(96,96,96)");
    buttonChangeDisplay->setStyleSheet("background-color: rgb(96,96,96)");

    buttonZoomUp->setIcon(QIcon(QString(WINDOWICONPATH) + QString("zoomup.png")));
    buttonZoomDown->setIcon(QIcon(QString(WINDOWICONPATH) + QString("zoomdown.png")));
    buttonReturnModel->setIcon(QIcon(QString(WINDOWICONPATH) + QString("home.png")));
    buttonChangeDisplay->setIcon(QIcon(QString(WINDOWICONPATH) + QString("curve.png")));
    buttonZoomUp->setFixedSize(40, 40);
    buttonZoomDown->setFixedSize(40, 40);
    buttonReturnModel->setFixedSize(40, 40);
    buttonChangeDisplay->setFixedSize(40, 40);

    buttonChangeDisplay->hide();

    QWidget* widgetOperation = new QWidget();
    QHBoxLayout* layoutPosition = new QHBoxLayout(m_robotModelWidget);
    layoutPosition->addWidget(widgetOperation, 0, Qt::AlignBottom | Qt::AlignRight);

    QHBoxLayout *layoutOperation = new QHBoxLayout(widgetOperation);
    layoutOperation->addWidget(buttonZoomUp);
    layoutOperation->addWidget(buttonZoomDown);
    layoutOperation->addWidget(buttonReturnModel);
    layoutOperation->addWidget(buttonChangeDisplay);

    buttonReturnModelClicked();

    m_timer = new QTimer(this);

    QObject::connect(buttonZoomUp, SIGNAL(clicked()), this, SLOT(buttonZoomUpClicked()));
    QObject::connect(buttonZoomDown, SIGNAL(clicked()), this, SLOT(buttonZoomDownClicked()));
    QObject::connect(buttonReturnModel, SIGNAL(clicked()), this, SLOT(buttonReturnModelClicked()));
    QObject::connect(buttonChangeDisplay, SIGNAL(clicked()), this, SLOT(buttonChangeDisplayClicked()));
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(updateRobotModelTimer()));

    m_timer->start(100);
}

RobotModel::~RobotModel(){

}

void RobotModel::updateRobotType(int type){
    m_robotModelWidget->updateRobotType(type);
}

void RobotModel::initViewPoint(float lo, float la, float di){
    m_longitude = lo;
    m_latitude = la;
    m_distance = di;
    m_robotModelWidget->changeLookAtParameter(m_longitude, m_latitude, m_distance);
    m_robotModelWidget->update();
}

void RobotModel::updateJoint(double *angle){
    m_robotModelWidget->changeJointValue(angle);
}

void RobotModel::updateTerminal(double *point){
    m_robotModelWidget->addCurvePoint(point);
}

void RobotModel::updatePaint(){
    m_robotModelWidget->changeLookAtParameter(m_longitude, m_latitude, m_distance);
    m_robotModelWidget->update();
}

void RobotModel::updateModelState(double *angle, double *point){
    m_robotModelWidget->changeJointValue(angle);
    m_robotModelWidget->addCurvePoint(point);
}

void RobotModel::buttonZoomUpClicked(){
    m_distance--;
    if(m_distance==0)
        m_distance=5;
    m_robotModelWidget->IncLookAtDistance();
}

void RobotModel::buttonZoomDownClicked(){
    m_distance++;
    m_robotModelWidget->DecLookAtDistance();
}

void RobotModel::buttonReturnModelClicked(){
    this->initViewPoint(-40, 0, 4); // 初始化视角
}

void RobotModel::buttonChangeDisplayClicked(){
    if(m_displayFlag){
        m_displayFlag = 0;
        buttonChangeDisplay->setIcon(QIcon(QString(WINDOWICONPATH) + QString("curve.png")));
    }
    else{
        m_displayFlag = 1;
        buttonChangeDisplay->setIcon(QIcon(QString(WINDOWICONPATH) + QString("robot.png")));
    }
    m_robotModelWidget->drawFlagChanged(m_displayFlag);
}

void RobotModel::updateRobotModelTimer(){ 
    int type =RobotClient::initance()->getRobotParameter().getRobotType();
    updateRobotType(type);

    vector<string> resvec;

    while(m_count == 50){ //每5秒执行一次
        FILE *fp;

        string tmp = string("ps -p ") +to_string(PID) + string(" \n") + string("echo $?");  //查找指定PID的进程（示教器进程）并输出结果，找到输出0，找不到输出1
        const char* start_jud = tmp.c_str();

        do{
            fp=popen(start_jud, "r");
            if(!fp){
                printf("fail to start shell\n");
            }else{
                char Tmp[256];
                while(fgets(Tmp, sizeof(Tmp), fp) != NULL){
                    if (Tmp[strlen(Tmp) - 1] == '\n') {
                                Tmp[strlen(Tmp) - 1] = '\0'; //去除换行符
                            }
                            resvec.push_back(Tmp);
                }
            }
        }
        while(!fp);
        pclose(fp);

        if(resvec[1] == "1")
        {
            qApp->quit();
        }
        m_count = 0;
    }

    RobotMotion m_RobotMotion = RobotClient::initance()->getRobotMotion();

    Joints m_CurrentJoints = m_RobotMotion.getCurrentJointPosition();
    Terminal m_CurrentTerminal = m_RobotMotion.getCurrentTerminal();

    double angle[6]={0},point[6]={0};
    for(int i=0;i<6;i++){
       angle[i]=m_CurrentJoints[static_cast<JOINTINDEX>(i)];
       point[i]=m_CurrentTerminal[static_cast<TERMINALINDEX>(i)];
    }

    m_count++;

    updateModelState(angle, point);
    updatePaint();
}

void RobotModel::mousePressEvent(QMouseEvent *event){
    mouseX = event->localPos().x();
    mouseY = event->localPos().y();
//    cout << "press mouse: " << event->localPos().x() << ", " << event->localPos().y() << endl;
}

void RobotModel::mouseMoveEvent(QMouseEvent *event){
    float newMouseX, newMouseY;
    newMouseX = event->localPos().x();
    newMouseY = event->localPos().y();

    m_longitude -= (newMouseX-mouseX) * 1.0 / this->size().width() * 360;
    m_latitude += (newMouseY-mouseY) * 1.0 / this->size().height() * 360;

    m_robotModelWidget->changeLookAtParameter(m_longitude, m_latitude, m_distance);

    mouseX = event->localPos().x();
    mouseY = event->localPos().y();
//    cout << "move mouse: " << event->localPos().x() << ", " << event->localPos().y() << endl;
}
