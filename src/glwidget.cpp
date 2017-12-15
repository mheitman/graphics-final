#include "glwidget.h"

#include "sphere.h"
#include "cs123_lib/resourceloader.h"
#include "cs123_lib/errorchecker.h"
#include "settings.h"
#include <QMouseEvent>
#include <iostream>
#include <QApplication>
#include "cs123_lib/objparser.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "camera/OrbitingCamera.h"

#define PI 3.14159265f

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent),
      m_program(0),
//      m_camera
      m_isDragging(false),
      m_camera(new OrbitingCamera()),
      m_sphere(nullptr)
{
//    ObjParser::load_obj("hi", m_vertices, m_uvs, m_vertIndices, m_uvIndices);
    ObjParser::load_obj("/Users/MaeHeitmann/Desktop/head.obj", m_vertices, m_uvs, m_vertIndices, m_uvIndices);
    std::cout << m_vertices.size() << std::endl;
    std::cout << m_uvs.size() << std::endl;
    std::cout << m_vertIndices.size() << std::endl;
    std::cout << m_uvIndices.size() << std::endl;
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL() {
    ResourceLoader::initializeGlew();
    resizeGL(width(), height());

    // START
    glEnable(GL_DEPTH_TEST);

    // Move the polygons back a bit so lines are still drawn even though they are coplanar with the
    // polygons they came from, which will be drawn before them.
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);

    // Calculate the orbiting camera matrices.
    m_camera->updateMatrices();
    // END

//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    // Set the color to set the screen when the color buffer is cleared.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Creates the shader program that will be used for drawing.
    m_program = ResourceLoader::createShaderProgram(":/shaders/shader.vert", ":/shaders/shader.frag");

    // Initialize sphere with radius 0.5 centered at origin.
    std::vector<GLfloat> sphereData = SPHERE_VERTEX_POSITIONS;
    m_sphere = std::make_unique<OpenGLShape>();

    m_sphere->setVertexData(&sphereData[0], sphereData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES, NUM_SPHERE_VERTICES);
    m_sphere->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_sphere->setAttribute(ShaderAttrib::NORMAL, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, true);
    m_sphere->buildVAO();
}

void GLWidget::paintGL() {
    ErrorChecker::printGLErrors("line 52");
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 eye = glm::vec3(0.f, 1.f, 4.f);
    glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
//    m_camera->updateMatrices();
//    m_camera->orientLook(camera.pos, camera.look, camera.up);
    //                    cam->setHeightAngle(camera.heightAngle);

//    glm::mat4 proj = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
//    glm::mat4 proj = m_camera->getProjectionMatrix();
//    glm::mat4 view = glm::lookAt(eye, center, up);
//    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 model(1.f);

    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    glViewport(0, 0, width() * ratio, height() * ratio);
    m_camera->setAspectRatio(static_cast<float>(width()) / static_cast<float>(height()));
    glm::mat4 proj = m_camera->getProjectionMatrix();
    glm::mat4 view = m_camera->getViewMatrix();
//    proj = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);

    glUseProgram(m_program);

    // Sets projection and view matrix uniforms.
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Sets uniforms that are controlled by the UI.
    glUniform1f(glGetUniformLocation(m_program, "shininess"), settings.shininess);
    glUniform1f(glGetUniformLocation(m_program, "lightIntensity"), settings.lightIntensity);
    glUniform3f(glGetUniformLocation(m_program, "lightColor"),
                settings.lightColor.redF(),
                settings.lightColor.greenF(),
                settings.lightColor.blueF());
    glUniform1f(glGetUniformLocation(m_program, "attQuadratic"), settings.attQuadratic);
    glUniform1f(glGetUniformLocation(m_program, "attLinear"), settings.attLinear);
    glUniform1f(glGetUniformLocation(m_program, "attConstant"), settings.attConstant);
    glUniform1f(glGetUniformLocation(m_program, "ambientIntensity"), settings.ambientIntensity);
    glUniform1f(glGetUniformLocation(m_program, "diffuseIntensity"), settings.diffuseIntensity);
    glUniform1f(glGetUniformLocation(m_program, "specularIntensity"), settings.specularIntensity);

    // Draws a sphere at the origin.
    model = glm::mat4(1.f);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_program, "color"),
                settings.sphereMColor.redF(),
                settings.sphereMColor.greenF(),
                settings.sphereMColor.blueF());
    m_sphere->draw();

    // TODO: Draw two more spheres. (Task 2)
    model = glm::translate(glm::vec3(-1.5, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_program, "color"),
                settings.sphereLColor.redF(),
                settings.sphereLColor.greenF(),
                settings.sphereLColor.blueF());
    m_sphere->draw();

    model = glm::translate(glm::vec3(1.5, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_program, "color"),
                settings.sphereRColor.redF(),
                settings.sphereRColor.greenF(),
                settings.sphereRColor.blueF());
    m_sphere->draw();

    glUseProgram(0);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_camera->mouseDown(10*event->x(), 10 * event->y());
        m_isDragging = true;
        update();
        std::cout << "ALERT MOUSE IS DOWN, ALERT, MOUSE, IS, DOWN" << std::endl;
         m_camera->updateMatrices();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        std::cout << event->x() <<" " << event->y() << std::endl;
        m_camera->mouseDragged(10 *event->x(), 10 *event->y());
        std::cout << "ALERT DRAGGGGGGINGGGGG" << std::endl;
        update();
         m_camera->updateMatrices();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDragging && event->button() == Qt::RightButton) {
        m_camera->mouseUp(10*event->x(), 10*event->y());
        m_isDragging = false;
        update();
         m_camera->updateMatrices();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    m_camera->mouseScrolled(event->delta());
    update();
     m_camera->updateMatrices();
}

void GLWidget::resizeEvent(QResizeEvent *event) {
    emit aspectRatioChanged();
}

void GLWidget::resetUpVector() {
    // Reset the up vector to the y axis
    glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);
//    if (fabs(glm::length(m_camera->getUp() - up)) > 0.0001f) {
//        m_camera->orientLook(
//                    m_camera->getPosition(),
//                    m_camera->getLook(),
//                    up);
//        update();
//    }
}


void GLWidget::setCameraAxisX() {
//    m_camera->orientLook(
//                glm::vec4(2.f, 0.f, 0.f, 1.f),
//                glm::vec4(-1.f, 0.f, 0.f, 0.f),
//                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void GLWidget::setCameraAxisY() {
//    m_camera->orientLook(
//                glm::vec4(0.f, 2.f, 0.f, 1.f),
//                glm::vec4(0.f, -1.f, 0.f, 0.f),
//                glm::vec4(0.f, 0.f, 1.f, 0.f));
    update();
}

void GLWidget::setCameraAxisZ() {
//    m_camera->orientLook(
//                glm::vec4(0.f, 0.f, 2.f, 1.f),
//                glm::vec4(0.f, 0.f, -1.f, 0.f),
//                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void GLWidget::setCameraAxonometric() {
//    m_camera->orientLook(
//                glm::vec4(2.f, 2.f, 2.f, 1.f),
//                glm::vec4(-1.f, -1.f, -1.f, 0.f),
//                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void GLWidget::updateCameraHeightAngle() {
    // The height angle is half the overall field of view of the camera
//    m_camera->setHeightAngle(settings.cameraFov / 2);
}

void GLWidget::updateCameraTranslation() {
    std::cout << "SDKLFJ:DLSKFJSD:LKF JSDLFD" << std::endl;
//    m_camera->translate(
//            glm::vec4(
//                settings.cameraPosX - m_oldPosX,
//                settings.cameraPosY - m_oldPosY,
//                settings.cameraPosZ - m_oldPosZ,
//                0));

//    m_oldPosX = settings.cameraPosX;
//    m_oldPosY = settings.cameraPosY;
//    m_oldPosZ = settings.cameraPosZ;
}

void GLWidget::updateCameraRotationU() {
//    m_camera->rotateU(settings.cameraRotU - m_oldRotU);
//    m_oldRotU = settings.cameraRotU;
}

void GLWidget::updateCameraRotationV() {
//    m_camera->rotateV(settings.cameraRotV - m_oldRotV);
//    m_oldRotV = settings.cameraRotV;
}

void GLWidget::updateCameraRotationN() {
//    m_camera->rotateW(settings.cameraRotN - m_oldRotN);
//    m_oldRotN = settings.cameraRotN;
}

void GLWidget::updateCameraClip() {
//    m_camera->setClip(settings.cameraNear, settings.cameraFar);
}
