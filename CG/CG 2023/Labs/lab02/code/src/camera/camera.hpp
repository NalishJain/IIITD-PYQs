#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <globals.hpp>
#include <utils/utils.hpp>

/*
Camera class that manipulates the view of the user. 
Can be moved with keys and looked around with mouse.
Responsible for setting up the view and perspective matrix.
Inspired from www.learnopengl.com
*/
class Camera
{
private:
    glm::vec3 cam_pos, cam_front, cam_up;
    glm::mat4 view_t, projection_t;
    const float SENSITIVITY = 0.1f;
    const float SPEED = 1.0f;
    float fov, near, far;
    float pitch,yaw;
    bool dragging;
    double prev_x, prev_y;
    void mouse_motion(double xpos, double ypos);
    void process_keys(GLFWwindow *window, float deltaTime);

public:
    void process_input(GLFWwindow *window, float deltaTime);
    Camera(glm::vec3 pos, glm::vec3 look_at, glm::vec3 up, float _fov, float near_clip, 
    float far_clip, GLFWwindow *window) : cam_pos(pos), cam_front(glm::normalize(look_at - pos)), cam_up(up), fov(_fov), near(near_clip), far(far_clip)
    {
        dragging = false;
        prev_x = (GLfloat) SCREEN_W/2;
        prev_y = (GLfloat)SCREEN_H / 2;
        pitch = -35.1519f;
        yaw = -40.6623f;
    }
    inline glm::vec3 getOrigin(){return cam_pos;}
    inline glm::vec3 getLookAt(){return cam_pos + cam_front;}
    inline glm::mat4 getProjection(){return projection_t;}
    inline glm::mat4 getView(){return view_t;}
    void setViewTransformation(unsigned int &shaderProgram);
    void setProjectionTransformation(unsigned int &shaderProgram);
    void setCamPos(unsigned int &shaderProgram);
};
#endif