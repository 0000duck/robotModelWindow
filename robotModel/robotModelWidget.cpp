#include "robotModelWidget.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

#define XOFFSET 0
#define YOFFSET 0
#define ZOFFSET 0
#define PI 3.1415926

#define MAXTRAJPOINTNUM 300

robotModelWidget::robotModelWidget(QWidget *parent):QOpenGLWidget(parent){
    for(int i=0; i<DOF; i++){
        joint[i] = 0;
    }
    longitude = 0;
    latitude = 0;
    distance = 3;
    m_type = 0;
    drawFlag = 0;

    vertices.reserve(7);
    for(int i=0; i<=DOF; i++){
        vertices[i] = new GLfloat[1];
    }


    curvePoint.resize(MAXTRAJPOINTNUM);
    for(list <TerminalPos>::iterator iter = curvePoint.begin(); iter != curvePoint.end(); iter++){
        (*iter).x = 0;
        (*iter).y = 0;
        (*iter).z = 0;
        (*iter).a = 0;
        (*iter).b = 0;
        (*iter).c = 0;
    }

    initializeModel();
}

robotModelWidget::~robotModelWidget(){
    for(int i=0; i<=DOF; i++){
        delete [] vertices[i];
    }
}

void robotModelWidget::initializeGL(){
    initializeOpenGLFunctions();

    glClearColor(0.14f, 0.32f, 0.71f, 1.0f);				// 背景
    glClearDepthf(1.0f);                                // 设置深度缓存
    glEnable(GL_DEPTH_TEST);							// 启用深度测试

    m_program = new QOpenGLShaderProgram(this);
    string vertPath = WINDOWMODELPATH + string("shader.vert");
    string fragPath = WINDOWMODELPATH + string("shader.frag");
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertPath.c_str());
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragPath.c_str());
    m_program->link();

    m_posAttr = m_program->attributeLocation("posAttr");
    m_norAttr = m_program->attributeLocation("norAttr");

    m_matrixUniform = m_program->uniformLocation("matrix");

    updateVBO();
}

void robotModelWidget::resizeGL(int w, int h){
    glViewport(0, 0, (GLint)w, (GLint)h);               //重置当前的视口
}

void robotModelWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存

    if(drawFlag == 0){
        draw3DModel();
    }
    else{
        drawCurve();
    }
}


void robotModelWidget::updateRobotType(int type){
    if(m_type == type){
        return;
    }
    m_type = type;
    initializeModel();

    m_program->bind();
    for(int i=0; i<=DOF; i++){
        VBO[i].bind();
        VBO[i].allocate(vertices[i], vertexList[i].size() * 6 * sizeof(GLfloat));
        VBO[i].release();
    }
    m_program->release();

    update();
}

void robotModelWidget::updateVBO(){
    m_program->bind();

    for(int i=0; i<=DOF; i++){
        VBO[i].create();
        VBO[i].bind();
        VBO[i].setUsagePattern(QOpenGLBuffer::StaticDraw);
        VBO[i].allocate(vertices[i], vertexList[i].size() * 6 * sizeof(GLfloat));

        VAO[i].create();
        VAO[i].bind();

        m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
        m_program->enableAttributeArray(m_posAttr);
        m_program->setAttributeBuffer(m_norAttr, GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
        m_program->enableAttributeArray(m_norAttr);

        VAO[i].release();
        VBO[i].release();
    }

    m_program->release();
}

void robotModelWidget::draw3DModel(){
    m_program->bind();

//    double lightz = cos(latitude * PI / 180) * cos(longitude * PI / 180);
//    double lightx = cos(latitude * PI / 180) * sin(longitude * PI / 180);
//    double lighty = sin(latitude * PI / 180);

    double eyez = distance * cos(latitude * PI / 180) * cos(longitude * PI / 180);
    double eyex = distance * cos(latitude * PI / 180) * sin(longitude * PI / 180);
    double eyey = distance * sin(latitude * PI / 180) + lookAtZP*scaleRatio;

    double centerz = 0;
    double centerx = 0;
    double centery = lookAtZP*scaleRatio;

    double upz = -sin(latitude * PI / 180) * cos(longitude * PI / 180);
    double upx = -sin(latitude * PI / 180) * sin(longitude * PI / 180);
    double upy = cos(latitude * PI / 180);

    glUniform3f(m_program->uniformLocation("dirLight.direction"), eyex, eyey, eyez);
    glUniform3f(m_program->uniformLocation("dirLight.ambient"), 0.5f, 0.5f, 0.5f);
    glUniform3f(m_program->uniformLocation("dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
    glUniform3f(m_program->uniformLocation("dirLight.specular"), 1.0f, 1.0f, 1.0f);

//    glUniform3f(m_program->uniformLocation("pointLights[0].position"), -1, -1, -1);
//    glUniform3f(m_program->uniformLocation("pointLights[0].ambient"), 1.0f, 1.0f, 1.0f);
//    glUniform3f(m_program->uniformLocation("pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
//    glUniform3f(m_program->uniformLocation("pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[0].constant"), 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[0].linear"), 0.09);
//    glUniform1f(m_program->uniformLocation("pointLights[0].quadratic"), 0.032);

//    glUniform3f(m_program->uniformLocation("pointLights[1].position"), 1, -1, -1);
//    glUniform3f(m_program->uniformLocation("pointLights[1].ambient"), 1.0f, 1.0f, 1.0f);
//    glUniform3f(m_program->uniformLocation("pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
//    glUniform3f(m_program->uniformLocation("pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[1].constant"), 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[1].linear"), 0.09);
//    glUniform1f(m_program->uniformLocation("pointLights[1].quadratic"), 0.032);

//    glUniform3f(m_program->uniformLocation("pointLights[2].position"), -1, 1, -1);
//    glUniform3f(m_program->uniformLocation("pointLights[2].ambient"), 1.0f, 1.0f, 1.0f);
//    glUniform3f(m_program->uniformLocation("pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
//    glUniform3f(m_program->uniformLocation("pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[2].constant"), 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[2].linear"), 0.09);
//    glUniform1f(m_program->uniformLocation("pointLights[2].quadratic"), 0.032);

//    glUniform3f(m_program->uniformLocation("pointLights[3].position"), 1, 1, -1);
//    glUniform3f(m_program->uniformLocation("pointLights[3].ambient"), 1.0f, 1.0f, 1.0f);
//    glUniform3f(m_program->uniformLocation("pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
//    glUniform3f(m_program->uniformLocation("pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[3].constant"), 1.0f);
//    glUniform1f(m_program->uniformLocation("pointLights[3].linear"), 0.09);
//    glUniform1f(m_program->uniformLocation("pointLights[3].quadratic"), 0.032);

    // Set material properties
//    gold
//    glUniform3f( m_program->uniformLocation("material.ambient"), 0.247250, 0.224500, 0.064500);
//    glUniform3f( m_program->uniformLocation("material.diffuse"), 0.346150, 0.314300, 0.090300);
//    glUniform3f( m_program->uniformLocation("material.specular"), 0.797357, 0.723991, 0.208006);
//    glUniform1f( m_program->uniformLocation("material.shininess"), 83.199997);

    //silver
    glUniform3f( m_program->uniformLocation("material.ambient"), 0.90980392, 0.90980392, 0.90980392);
    glUniform3f( m_program->uniformLocation("material.diffuse"), 0.54509804, 0.54509804, 0.47843137);
    glUniform3f( m_program->uniformLocation("material.specular"), 0.75686275, 0.80392157, 0.80392157);
    glUniform1f( m_program->uniformLocation("material.shininess"), 1.0f);

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.perspective(45.0f, this->width()*1.0/this->height(), 0.1f, 100.0f);


//    matrix.ortho(-1.0f, +1.0f, -1.0f, 1.0f, 0.0f, 15.0f);
    matrix.lookAt(QVector3D(eyex, eyey, eyez), QVector3D(centerx, centery, centerz), QVector3D(upx, upy, upz));
    m_program->setUniformValue(m_matrixUniform, matrix);

    glUniform3f( m_program->uniformLocation("viewPos"), eyex, eyey, eyez);

    for(int i=0; i<=DOF; i++){
        VAO[i].bind();
        glDrawArrays(GL_TRIANGLES, 0, vertexList[i].size());
        VAO[i].release();
        if(i != DOF){
            if(fabs(xAxis[i]+yAxis[i]+zAxis[i]) == 1){
                matrix.translate(xTran[i]*scaleRatio, yTran[i]*scaleRatio, zTran[i]*scaleRatio);
                matrix.rotate(joint[i], xAxis[i], yAxis[i], zAxis[i]);
                matrix.translate(-xTran[i]*scaleRatio, -yTran[i]*scaleRatio, -zTran[i]*scaleRatio);
                m_program->setUniformValue(m_matrixUniform, matrix);
            }
            else{
                matrix.translate(xTran[i]*scaleRatio, yTran[i]*scaleRatio, zTran[i]*scaleRatio);
                matrix.translate((xAxis[i]*joint[i]/2)*scaleRatio, (yAxis[i]*joint[i]/2)*scaleRatio, (zAxis[i]*joint[i]/2)*scaleRatio);
                matrix.translate(-xTran[i]*scaleRatio, -yTran[i]*scaleRatio, -zTran[i]*scaleRatio);
                m_program->setUniformValue(m_matrixUniform, matrix);
            }
        }
    }

    m_program->release();
}

void robotModelWidget::drawCurve(){
    m_program->bind();

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.perspective(45.0f, this->width()* 1.0/this->height(), 0.1f, 100.0f);
    double eyez = distance * cos(latitude * PI / 180) * cos(longitude * PI / 180);
    double eyex = distance * cos(latitude * PI / 180) * sin(longitude * PI / 180);
    double eyey = distance * sin(latitude * PI / 180) + lookAtZP*scaleRatio;
    double centerx = 0;
    double centery = lookAtZP*scaleRatio;
    double centerz = 0;
    double upz = -sin(latitude * PI / 180) * cos(longitude * PI / 180);
    double upx = -sin(latitude * PI / 180) * sin(longitude * PI / 180);
    double upy = cos(latitude * PI / 180);
//    matrix.ortho(-1.0f, +1.0f, -1.0f, 1.0f, 0.0f, 15.0f);
    matrix.lookAt(QVector3D(eyex, eyey, eyez), QVector3D(centerx, centery, centerz), QVector3D(upx, upy, upz));
    m_program->setUniformValue(m_matrixUniform, matrix);

    glUniform3f( m_program->uniformLocation("viewPos"), eyex, eyey, eyez);

    //base frame
    vector<Vertex3f> clx;
    clx.push_back(Vertex3f({0, 0, 0}));
    clx.push_back(Vertex3f({0, 0, 1}));
    drawLine(clx, Vertex4f({1, 0, 0, 1}), 4);

    vector<Vertex3f> cly;
    cly.push_back(Vertex3f({0, 0, 0}));
    cly.push_back(Vertex3f({1, 0, 0}));
    drawLine(cly, Vertex4f({1, 0, 1, 1}), 4);

    vector<Vertex3f> clz;
    clz.push_back(Vertex3f({0, 0, 0}));
    clz.push_back(Vertex3f({0, 1, 0}));
    drawLine(clz, Vertex4f({0, 1, 0, 1}), 4);

    // Set material properties
    list <TerminalPos>::iterator iter;
    for(iter = curvePoint.begin(); iter != curvePoint.end(); iter++){
        float tempX = (*iter).y;
        float tempY = (*iter).z;
        float tempZ = (*iter).x;
        drawPoint(Vertex3f({tempX/1000, tempY/1000, tempZ/1000}), Vertex4f({1, 0, 0, 1}), 2);
    }

    TerminalPos currentPoint = curvePoint.back();

    matrix.translate(currentPoint.y/1000, currentPoint.z/1000, currentPoint.x/1000);
    matrix.rotate(currentPoint.b, 0, 1, 0);
    matrix.rotate(90 - currentPoint.a, 1, 0, 0);
    matrix.rotate(currentPoint.c, 0, 0, 1);
    m_program->setUniformValue(m_matrixUniform, matrix);

    vector<Vertex3f> olx;
    olx.push_back(Vertex3f({0, 0, 0}));
    olx.push_back(Vertex3f({0, 0, 0.2}));
    drawLine(olx, Vertex4f({1, 0, 0, 1}), 2);

    vector<Vertex3f> oly;
    oly.push_back(Vertex3f({0, 0, 0}));
    oly.push_back(Vertex3f({0.2, 0, 0}));
    drawLine(oly, Vertex4f({1, 0, 1, 1}), 2);

    vector<Vertex3f> olz;
    olz.push_back(Vertex3f({0, 0, 0}));
    olz.push_back(Vertex3f({0, 0.2, 0}));
    drawLine(olz, Vertex4f({0, 1, 0, 1}), 2);

    m_program->release();
}

void robotModelWidget::drawLine(std::vector<Vertex3f> tempVertexList, Vertex4f tempColor, float lineW){
    // Set material properties
    glUniform3f( m_program->uniformLocation("material.ambient"), tempColor.x, tempColor.y, tempColor.z);
    glUniform3f( m_program->uniformLocation("material.diffuse"), tempColor.x, tempColor.y, tempColor.z);
    glUniform3f( m_program->uniformLocation("material.specular"), tempColor.x, tempColor.y, tempColor.z);
    glUniform1f( m_program->uniformLocation("material.shininess"), 100.0f);

    GLfloat posPoint[] = {tempVertexList[0].x, tempVertexList[0].y, tempVertexList[0].z,
                           tempVertexList[1].x, tempVertexList[1].y, tempVertexList[1].z};
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, posPoint);
    glEnableVertexAttribArray(0);

    glLineWidth(lineW);
    glDrawArrays(GL_LINES, 0, 2);

    glDisableVertexAttribArray(0);
}

#if defined(QT_OPENGL_ES_2)
#define SETPOINTSIZE
#else
#ifdef _WIN32
#define SETPOINTSIZE
#else
#define SETPOINTSIZE glPointSize(pointW)
#endif
#endif

void robotModelWidget::drawPoint(Vertex3f tempVertex, Vertex4f tempColor, float pointW){
    // Set material properties
    glUniform3f( m_program->uniformLocation("material.ambient"), tempColor.x, tempColor.y, tempColor.z);
    glUniform3f( m_program->uniformLocation("material.diffuse"), tempColor.x, tempColor.y, tempColor.z);
    glUniform3f( m_program->uniformLocation("material.specular"), tempColor.x, tempColor.y, tempColor.z);
    glUniform1f( m_program->uniformLocation("material.shininess"), 100.0f);

    GLfloat posPoint[] = {tempVertex.x, tempVertex.y, tempVertex.z};
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, posPoint);
    glEnableVertexAttribArray(0);

    SETPOINTSIZE;
    glDrawArrays(GL_POINTS, 0, 1);

    glDisableVertexAttribArray(0);
}

void robotModelWidget::drawFlagChanged(int state){
    drawFlag = state;
}

void robotModelWidget::addCurvePoint(double* point){
    TerminalPos tempPoint({point[0]*scaleRatio, point[1]*scaleRatio, point[2]*scaleRatio, point[3], point[4], point[5]});
    curvePoint.push_back(tempPoint);
    if(curvePoint.size()>MAXTRAJPOINTNUM){
        curvePoint.pop_front();
    }
}

void robotModelWidget::changeJointValue(double* angle){
    for(int i=0; i<DOF; i++){
        joint[i] = angle[i];
    }
}

void robotModelWidget::changeLookAtParameter(float lo, float la, float di){
    longitude = lo;
    latitude = la;
//    distance = 1.0 / di * 50;
    distance = di;
}

void robotModelWidget::changeLookAtDistance(float di){
//    distance = 1.0 / di * 50;
    distance = di;
}

void robotModelWidget::IncLookAtDistance(){
    if(distance > 0){
        distance -= 1;
    }
    if(distance==0)
        distance=5;
}

void robotModelWidget::DecLookAtDistance(){
    distance += 1;
}

void robotModelWidget::initializeModel(){
    string typestr;
    switch (m_type) {
    case 0:
        typestr = "norm/";
        break;
    case 1:
        typestr = "ur/";
        break;
    case 2:
        typestr = "scara/";
        break;
    default:
        break;
    }

    string pathModelSize;
    string pathLink0, pathLink1, pathLink2, pathLink3, pathLink4, pathLink5, pathLink6;

    pathModelSize = WINDOWMODELPATH + typestr + "modelSize.robotf";
    pathLink0 = WINDOWMODELPATH + typestr + "link0.STL";
    pathLink1 = WINDOWMODELPATH + typestr + "link1.STL";
    pathLink2 = WINDOWMODELPATH + typestr + "link2.STL";
    pathLink3 = WINDOWMODELPATH + typestr + "link3.STL";
    pathLink4 = WINDOWMODELPATH + typestr + "link4.STL";
    pathLink5 = WINDOWMODELPATH + typestr + "link5.STL";
    pathLink6 = WINDOWMODELPATH + typestr + "link6.STL";

    FILE *file;
    if(!(file=fopen(pathModelSize.c_str(),"rb+"))){
        printf("failed to open model size file\n");
        exit(1);
    }

    int ret;
    ret = fscanf(file, "%f", &scaleRatio);
    ret = fscanf(file, "%f", &lookAtZP);
    for(int i=0; i<6; i++)
        ret = fscanf(file, "%f %f %f", &xTran[i], &yTran[i], &zTran[i]);
    for(int i=0; i<6; i++)
        ret = fscanf(file, "%f %f %f", &xAxis[i], &yAxis[i], &zAxis[i]);

    fclose(file);

    vertexList.clear();
    normalList.clear();
    loadSTLFile(pathLink0.c_str());
    loadSTLFile(pathLink1.c_str());
    loadSTLFile(pathLink2.c_str());
    loadSTLFile(pathLink3.c_str());
    loadSTLFile(pathLink4.c_str());
    loadSTLFile(pathLink5.c_str());
    loadSTLFile(pathLink6.c_str());

    for(int i=0; i<=DOF; i++){
        delete [] vertices[i];
        vertices[i] = new GLfloat[vertexList[i].size() * 6];
        for(int j=0; j<vertexList[i].size(); j++){
            vertices[i][6*j] = vertexList[i][j].x;
            vertices[i][6*j+1] = vertexList[i][j].y;
            vertices[i][6*j+2] = vertexList[i][j].z;
            vertices[i][6*j+3] = normalList[i][j/3].x;
            vertices[i][6*j+4] = normalList[i][j/3].y;
            vertices[i][6*j+5] = normalList[i][j/3].z;
        }
    }
}

void robotModelWidget::loadSTLFile(const char *path){
    FILE* file;
    if((file=fopen(path,"rb"))==NULL){
        throw string("打开模型文件出错");
    }

    long lSize;
    char* buffer;
    size_t result;

    fseek(file, 0, SEEK_END);
    lSize = ftell(file);
    rewind(file);

    buffer = (char*)malloc(sizeof(char)*lSize);
    if (buffer == NULL){
        throw string("读取模型文件出错");
    }

    result = fread(buffer, 1, lSize, file);
    if (result != lSize){
        throw string("读取模型文件出错");
    }

    fclose(file);

    ios::sync_with_stdio(false);
    vector<Vertex3f> tempVertexList;
    vector<Vertex3f> tempNormalList;
    if (buffer[79]!='\0'){
        readAsciiSTL(buffer, tempNormalList, tempVertexList);
    }
    else{
        readBinarySTL(buffer, tempNormalList, tempVertexList);
    }
    vertexList.push_back(tempVertexList);
    normalList.push_back(tempNormalList);
    ios::sync_with_stdio(true);
    free(buffer);
}

void robotModelWidget::readAsciiSTL(const char *buffer, vector<Vertex3f>& tempNormalList, vector<Vertex3f>& tempVertexList){
    float x, y, z;
    int i;
    string name, useless;
    stringstream ss(buffer);
//    ss >> name >> name;
//    ss.get();
    getline(ss, useless);
    do {
        ss >> useless;
        if (useless != "facet"){
            break;
        }
        ss >> useless >> x >> y >> z;
        tempNormalList.push_back(Vertex3f({x, y, z}));
        getline(ss, useless);
        getline(ss, useless);
        for (i = 0; i < 3; i++){
            ss >> useless >> x >> y >> z;
            tempVertexList.push_back(Vertex3f({-(x+XOFFSET)*scaleRatio, (z+YOFFSET)*scaleRatio, (y+ZOFFSET)*scaleRatio}));
        }
        getline(ss, useless);
        getline(ss, useless);
        getline(ss, useless);
    } while (1);
}

void robotModelWidget::readBinarySTL(const char *buffer, vector<Vertex3f>& tempNormalList, vector<Vertex3f>& tempVertexList){
    const char* p = buffer;
    char name[80];
    int i, j;
    memcpy(name, p, 80);
    p += 80;
    int unTriangles= cpyint(p);
    for (i = 0; i < unTriangles; i++){
        tempNormalList.push_back(Vertex3f({cpyfloat(p), cpyfloat(p), cpyfloat(p)}));
        for (j = 0; j < 3; j++)//读取三顶点
            tempVertexList.push_back(Vertex3f({cpyfloat(p)*scaleRatio, cpyfloat(p)*scaleRatio, cpyfloat(p)*scaleRatio}));
        p += 2;//跳过尾部标志
    }
}

int robotModelWidget::cpyint(const char *&p){
    int cpy;
    char* memwriter = (char*)&cpy;
    memcpy(memwriter, p, 4);
    p += 4;
    return cpy;
}

float robotModelWidget::cpyfloat(const char *&p){
    float cpy;
    char* memwriter = (char*)&cpy;
    memcpy(memwriter, p, 4);
        p += 4;
    return cpy;
}
