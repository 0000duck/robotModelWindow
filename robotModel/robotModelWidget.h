#ifndef ROBOTMODELWIDGET_H
#define ROBOTMODELWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QScreen>
#include <QMouseEvent>
#include <list>
#include <string>
#include <vector>
#include <QString>
#include <QApplication>
#include <QDir>

#define DOF 6
#define WINDOWICONPATH (qApp->applicationDirPath() + QString("/config/robotIcon/")).toStdString().c_str()
#define WINDOWMODELPATH (qApp->applicationDirPath() + QString("/config/robotSTL/")).toStdString().c_str()

struct Vertex3f{
    float x, y, z;
};

struct Vertex4f{
    float x, y, z, d;
};

struct TerminalPos{
    double x, y, z, a, b, c;
};

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QMouseEvent;
class robotModelWidget : public QOpenGLWidget, protected QOpenGLFunctions{
    Q_OBJECT
public:
    robotModelWidget(QWidget *parent = 0);
    ~robotModelWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int w,int h);

    void updateRobotType(int type);
    void changeJointValue(double* angle);
    void addCurvePoint(double* point);
    void changeLookAtParameter(float, float, float);
    void changeLookAtDistance(float);
    void IncLookAtDistance();
    void DecLookAtDistance();
    void drawFlagChanged(int);

private:
    void initializeModel();

    // draw robot model in STL
    void draw3DModel();

    // draw trajectory
    void drawCurve();
    void drawLine(std::vector<Vertex3f> tempVertexList, Vertex4f tempColor, float lineW);
    void drawPoint(Vertex3f tempVertex, Vertex4f tempColor, float pointW);

    //read robot model in STL
    void loadSTLFile(const char* path);
    void readAsciiSTL(const char *buffer, std::vector<Vertex3f>& tempNormalList, std::vector<Vertex3f>& tempVertexList);
    void readBinarySTL(const char *buffer, std::vector<Vertex3f>& tempNormalList, std::vector<Vertex3f>& tempVertexList);
    int cpyint(const char*& p);
    float cpyfloat(const char*& p);

    // update VBO parameter
    void updateVBO();

    int m_type;

    // robot model size
    float scaleRatio;
    float lookAtZP;
    float xTran[6];
    float yTran[6];
    float zTran[6];
    float xAxis[6];
    float yAxis[6];
    float zAxis[6];

    // robot model vertex
    std::vector<std::vector<Vertex3f> > vertexList;
    std::vector<std::vector<Vertex3f> > normalList;

    // opengl
    std::vector<GLfloat*> vertices;
    QOpenGLBuffer VBO[7];
    QOpenGLVertexArrayObject VAO[7];

    double joint[DOF];  // robot model angle
    float longitude, latitude, distance;   // view point
    std::list <TerminalPos> curvePoint;     // terminal point list
    int drawFlag;   // model or trajectory flag

    QOpenGLShaderProgram *m_program;
    GLuint m_posAttr;
    GLuint m_norAttr;

    GLuint m_matrixUniform;
    GLuint m_regmatUniform;
};

#endif
