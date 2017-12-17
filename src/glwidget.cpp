#include "glwidget.h"

#define BUFSIZE 1024

#include "sphere.h"
#include "cs123_lib/resourceloader.h"
#include "cs123_lib/errorchecker.h"
#include "settings.h"
#include <QMouseEvent>
#include <iostream>
#include <QApplication>
#include "cs123_lib/objparser.h"
#include "cs123_lib/textureloader.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "camera/OrbitingCamera.h"

#include <fstream>
#include <glm/gtx/string_cast.hpp>

#define PI 3.14159265f

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent),
      m_program(0),
      m_isDragging(false),
      m_camera(new OrbitingCamera()),
      m_sphere(nullptr),
      vao_handle(-1),
      positions_vbo(-1),
      uvs_vbo(-1),
      ibo_handle(-1)
{
    bool IS_MAE_RUNNING = false;

    char ben_path[] = "/Users/weissmann/Desktop/final_graphics/graphics-final/head-lee-perry-smith/head.obj";
    char mae_path[] = "/Users/MaeHeitmann/Desktop/head.obj";

    ObjParser::load_obj(IS_MAE_RUNNING ? mae_path : ben_path, m_vertices, m_uvs, m_tangents, m_bitangents, m_indices);
    std::cout << m_vertices.size() << " "
              << m_uvs.size() << " "
              << m_tangents.size() << " "
              << m_bitangents.size() << " "  << std::endl;
    ErrorChecker::printGLErrors("loadingtex gl");
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL() {
    ErrorChecker::printGLErrors("pre initgl");
    ResourceLoader::initializeGlew();
    ErrorChecker::printGLErrors("init glew");
    resizeGL(width(), height());

    ErrorChecker::printGLErrors("resize gl");

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);

    // Calculate the orbiting camera matrices.
    m_camera->updateMatrices();

    glEnable(GL_DEPTH_TEST);

    // Set the color to set the screen when the color buffer is cleared.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Creates the shader program that will be used for drawing.
    m_program = ResourceLoader::createShaderProgram(":/shaders/shader.vert", ":/shaders/shader.frag");

    initializeShape();

    TextureLoader::loadTexture(":/head-lee-perry-smith/source/lambertian.jpg", 0, tex_handle);
    TextureLoader::loadTexture(":/head-lee-perry-smith/source/out_bent_normals.bmp", 1, normal_handle);
    TextureLoader::loadTexture(":/head-lee-perry-smith/source/bump-lowRes.jpg", 2, bump_handle);

    ErrorChecker::printGLErrors("post initgl");
}

void GLWidget::paintGL() {
    ErrorChecker::printGLErrors("pre paintgl");
    glUseProgram(m_program);       // Installs the shader program. You'll learn about this later.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clears the color buffer. (i.e. Sets the screen to black.)
    glm::mat4 model(1.f);

    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    glViewport(0, 0, width() * ratio, height() * ratio);
    m_camera->setAspectRatio(static_cast<float>(width()) / static_cast<float>(height()));
    glm::mat4 proj = m_camera->getProjectionMatrix();
    glm::mat4 view = m_camera->getViewMatrix();
    proj = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);

    ErrorChecker::printGLErrors("post setup");

    // Sets projection and view matrix uniforms.
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

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
    glUniform3f(glGetUniformLocation(m_program, "color"), 1, 1, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // TODO [Task 8-10]: Draw shapes in the appropriate switch case.
    glBindVertexArray(vao_handle);

    // Position
    glEnableVertexAttribArray(ShaderAttrib::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
    glVertexAttribPointer(ShaderAttrib::POSITION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // UVs
    glEnableVertexAttribArray(ShaderAttrib::TEXCOORD0);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
    glVertexAttribPointer(ShaderAttrib::TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Tangents
    glEnableVertexAttribArray(ShaderAttrib::TANGENT);
    glBindBuffer(GL_ARRAY_BUFFER, tangents_vbo);
    glVertexAttribPointer(ShaderAttrib::TANGENT, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Bitangents
    glEnableVertexAttribArray(ShaderAttrib::BINORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, bitangents_vbo);
    glVertexAttribPointer(ShaderAttrib::BINORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Texture
    glUniform1i(glGetUniformLocation(m_program, "tex_sampler"), 0);
    glUniform1i(glGetUniformLocation(m_program, "normal_sampler"), 1);
    glUniform1i(glGetUniformLocation(m_program, "bump_sampler"), 2);

    ErrorChecker::printGLErrors("post tex");

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_handle);

    // Draw
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GL_UNSIGNED_INT), GL_UNSIGNED_INT, (void*)0);


    // Unbind everything
    glBindVertexArray(0);
    glUseProgram(0); // Uninstalls the shader program.

    ErrorChecker::printGLErrors("end paintgl");
}

void GLWidget::fillVBO(GLuint vbo_handle, std::vector<float> input) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, input.size() * sizeof(float), input.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLWidget::initializeShape() {

    // generate handles
    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &positions_vbo);
    glGenBuffers(1, &uvs_vbo);
    glGenBuffers(1, &tangents_vbo);
    glGenBuffers(1, &bitangents_vbo);
    glGenBuffers(1, &ibo_handle);

    glBindVertexArray(vao_handle);

    // FILL THE VBO WITH Data
    fillVBO(positions_vbo, m_vertices);
    fillVBO(uvs_vbo, m_uvs);
    fillVBO(tangents_vbo, m_tangents);
    fillVBO(bitangents_vbo, m_bitangents);

    // FILL THE IBO WITH INDICES
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ErrorChecker::printGLErrors("post init shape");
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_camera->mouseDown(event->x(), event->y());
        m_isDragging = true;
        update();
        std::cout << "ALERT MOUSE IS DOWN, ALERT, MOUSE, IS, DOWN" << std::endl;
        m_camera->updateMatrices();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        std::cout << event->x() <<" " << event->y() << std::endl;
        m_camera->mouseDragged(event->x(), event->y());
        std::cout << "ALERT DRAGGGGGGINGGGGG" << std::endl;
        update();
        m_camera->updateMatrices();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDragging && event->button() == Qt::RightButton) {
        m_camera->mouseUp(event->x(), event->y());
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
