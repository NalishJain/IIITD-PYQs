#include "camera.hpp"

void Camera::setCamPos(unsigned int &shader_program)
{
    glUseProgram(shader_program);
    unsigned int eye_pos_uniform = getUniform(shader_program, "eye_pos");
    glUniform3f(eye_pos_uniform, cam_pos.x, cam_pos.y, cam_pos.z);
}

void Camera::setViewTransformation(unsigned int &shader_program)
{
    view_t = glm::lookAt(glm::vec3(cam_pos), glm::vec3(cam_pos + cam_front), cam_up);
    glUseProgram(shader_program);
    unsigned int vView_uniform = getUniform(shader_program, "vView");
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(view_t));
}

void Camera::setProjectionTransformation(unsigned int &shader_program)
{

    projection_t = glm::perspective(fov, (GLfloat)SCREEN_W / (GLfloat)SCREEN_H, near, far);
    glUseProgram(shader_program);
    unsigned int vProjection_uniform = getUniform(shader_program, "vProjection");
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projection_t));
}

void Camera::process_keys(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        deltaTime = 10 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cam_pos += deltaTime * SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam_pos += deltaTime * SPEED * cam_front;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cam_pos -= deltaTime * SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam_pos -= deltaTime * SPEED * cam_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam_pos -= deltaTime * SPEED * glm::normalize(glm::cross(cam_front, cam_up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam_pos += deltaTime * SPEED * glm::normalize(glm::cross(cam_front, cam_up));
}

//Taken from learnopengl.com camera tutorial.
void Camera::mouse_motion(double xpos, double ypos)
{
    if (dragging)
    {
        //Rotating
        float xoffset = xpos - prev_x;
        float yoffset = prev_y - ypos;
        prev_x = xpos;
        prev_y = ypos;

        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        yaw += xoffset;
        pitch += yoffset;

        //clips to -89 to 89
        pitch = std::max(std::min(pitch, 89.0f), -89.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cam_front = glm::normalize(direction);
    }
}

void Camera::process_input(GLFWwindow *window, float deltaTime)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (dragging)
            mouse_motion(xpos, ypos);
        else
        {
            prev_x = xpos;
            prev_y = ypos;
            dragging = true;
        }
    }
    else
        dragging = false;
    process_keys(window, deltaTime);
}