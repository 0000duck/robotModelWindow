#ifndef ROBOTMODEL_H
#define ROBOTMODEL_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QTimer>
#include "robotModelWidget.h"

class robotModelWidget;
class QPushButton;
class QRadioButton;
class QVBoxLayout;
class QHBoxLayout;
class QTimer;
class RobotModel : public QWidget {
    Q_OBJECT
public:
    RobotModel(QWidget *parent = 0);
    ~RobotModel();
    void initViewPoint(float, float, float);
    void updateJoint(double*);
    void updateTerminal(double*);
    void updatePaint();
    void updateModelState(double*, double*);
    void updateRobotType(int type);

private:
    robotModelWidget *m_robotModelWidget;
    QPushButton *buttonZoomUp;
    QPushButton *buttonZoomDown;
    QPushButton *buttonReturnModel;
    QPushButton *buttonChangeDisplay;

    QTimer* m_timer;    // 定时时钟

    float m_longitude, m_latitude, m_distance;  // 视角
    int m_displayFlag = 0;  // 0:机器人 1:曲线

    int m_count = 0;

private slots:
    void buttonZoomUpClicked();
    void buttonZoomDownClicked();
    void buttonReturnModelClicked();
    void buttonChangeDisplayClicked();
    void updateRobotModelTimer();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    float mouseX, mouseY;
};

#endif
