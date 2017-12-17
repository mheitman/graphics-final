#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <memory>

#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include "sphere.h"
#include "camera/OrbitingCamera.h"

class OpenGLShape;

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

public slots:
    void resetUpVector();
    void setCameraAxisX();
    void setCameraAxisY();
    void setCameraAxisZ();
    void setCameraAxonometric();

    // These will be called whenever the corresponding UI elements are updated on the Camtrans dock
    void updateCameraHeightAngle();
    void updateCameraTranslation();
    void updateCameraRotationU();
    void updateCameraRotationV();
    void updateCameraRotationN();
    void updateCameraClip();

protected:
    void initializeGL();
    void initializeShape();
    void paintGL();
    void resizeGL(int w, int h);

    void fillVBO(GLuint vbo_handle, std::vector<float> input);

    // Overridden from QWidget
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

signals:
    void aspectRatioChanged();

private:
    /** ID for the shader program. */
    GLuint m_program;
    std::unique_ptr<OrbitingCamera> m_camera;
    bool m_isDragging;

    std::unique_ptr<OpenGLShape> m_sphere;
    std::vector<float> m_vertices;
    std::vector<float> m_uvs;
    std::vector<float> m_tangents;
    std::vector<float> m_bitangents;
    std::vector<unsigned int> m_indices;
    GLuint vao_handle;
    GLuint positions_vbo;
    GLuint uvs_vbo;
    GLuint tangents_vbo;
    GLuint bitangents_vbo;
    GLuint ibo_handle;

    GLuint tex_handle;
    GLuint normal_handle;
    GLuint bump_handle;
};

#endif // GLWIDGET_H
