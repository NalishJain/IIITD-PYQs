/*References
  Trackball: http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
*/

#include "utils.h"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_SWIZZLE

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "skybox.h"
#include "sphere.h"

#define ENABLE_REFLECTIVE_SPHERE 1

//Globals
int screen_width = 1280, screen_height=1280;
double oldX, oldY, currentX, currentY;
bool isDragging=false;
GLuint texCube;
SkyBox *skybox;
Sphere *reflectiveSphere;

void initResources();
void createCubeMap(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint *tex_cube);
bool loadImageToTexture(GLuint texture, GLenum textureType, GLenum textureSubType, const char* filename);

glm::vec3 getTrackBallVector(double x, double y);

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    initResources();

    oldX = oldY = currentX = currentY = 0.0;
    int prevLeftButtonState = GLFW_RELEASE;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Get current mouse position
        int leftButtonState = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
        double x,y;
        glfwGetCursorPos(window,&x,&y);
        if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_RELEASE){
            isDragging = true;
            currentX = oldX = x;
            currentY = oldY = y;
        }
        else if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_PRESS){
            currentX = x;
            currentY = y;
        }
        else if(leftButtonState == GLFW_RELEASE && prevLeftButtonState == GLFW_PRESS){
            isDragging = false;
        }

        // Rotate based on mouse drag movement
        prevLeftButtonState = leftButtonState;
        if(isDragging && (currentX !=oldX || currentY != oldY))
        {
            glm::vec3 va = getTrackBallVector(oldX, oldY);
            glm::vec3 vb = getTrackBallVector(currentX, currentY);

            float angle = acos(std::min(1.0f, glm::dot(va,vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);

            skybox->rotate(angle, axis_in_camera_coord);
#if ENABLE_REFLECTIVE_SPHERE
            reflectiveSphere->rotate(angle, axis_in_camera_coord);
#endif

            oldX = currentX;
            oldY = currentY;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        skybox->setProjectionMatrix(display_w, display_h);
#if ENABLE_REFLECTIVE_SPHERE
        reflectiveSphere->setProjectionMatrix(display_w, display_h);
#endif

        
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox->render();
#if ENABLE_REFLECTIVE_SPHERE
        reflectiveSphere->render();
#endif


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    cleanup(window);

    return 0;
}

void initResources()
{
	// Setup cubeMap texture
	createCubeMap(	
			"./res/cubemap/negz.jpg", 
			"./res/cubemap/posz.jpg",
			"./res/cubemap/negy.jpg",
			"./res/cubemap/posy.jpg",
			"./res/cubemap/negx.jpg",
			"./res/cubemap/posx.jpg",
			&texCube);
	skybox = new SkyBox(screen_width, screen_height, 10.0, "./shaders/vshaderCubemap.vs", "./shaders/fshaderCubemap.fs", &texCube);
						
#if ENABLE_REFLECTIVE_SPHERE
	reflectiveSphere = new Sphere(screen_width, screen_height, 10.0, "./shaders/vshaderSphere.vs", "./shaders/fshaderSphere.fs", &texCube);
#endif
}

void createCubeMap(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint *tex_cube)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex_cube);
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, top));
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, bottom));
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
	assert(loadImageToTexture(*tex_cube, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
	//Texture format
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool loadImageToTexture(GLuint texture, GLenum textureType, GLenum textureSubType, const char* filename)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if(image_data == NULL) return false;

	glBindTexture(textureType, texture);

    glTexImage2D(textureSubType, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
	return true;
}

glm::vec3 getTrackBallVector(double x, double y)
{
	glm::vec3 p = glm::vec3(2.0*x/screen_width - 1.0, 2.0*y/screen_height - 1.0, 0.0); //Normalize to [-1, +1]
	p.y = -p.y; //Invert Y since screen coordinate and OpenGL coordinates have different Y directions.

	float mag2 = p.x*p.x + p.y*p.y;
	if(mag2 <= 1.0f)
		p.z = sqrtf(1.0f - mag2);
	else
		p = glm::normalize(p); //Nearest point, close to the sides of the trackball
	return p;
}

