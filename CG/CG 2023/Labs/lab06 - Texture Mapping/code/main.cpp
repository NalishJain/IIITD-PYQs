#include <stdio.h>
#include <iostream>

// Image Loading
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int size;
int width, height, nrChannels;

glm::mat4 proj = glm::mat4(1.0f);
glm::vec3 Position = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view=glm::lookAt(Position, Position+Orientation, Up);


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

const char * setGLSLVersion(){
    #if __APPLE__
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

    return glsl_version;
}


int main(int, char**)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Lab 6: Implementing Textures", NULL, NULL);
    
    glfwMakeContextCurrent(window);
    glewInit();
	const char * glsl_version = setGLSLVersion();
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Define Vertex Shader
    const char *vertexShaderSource = "#version 330 core\n"              
    "layout (location = 0) in vec4 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "uniform float scale;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = vec2(aTexCoord);"
    "}\0";

    // Define Fragment Shader
    const char *fragmentShaderSource = "#version 330 core\n"            
    " out vec4 FragColor;\n"
    " in vec2 TexCoord;"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\0";


    // Compiling Vertex Shader
    unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success){
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

    // Compiling Fragment Shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
	{
	    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::Fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

    // Linking Vertex and Fragment Shader
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);

	// Checking
	if(!success)
	{
	    glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::Shaderprogram::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// float vertices[] =

    // For Cube
        float vertices[] = {
            // Position Coords  // Texture Coords         
            // Front Face
            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,// lower-left corner
             0.5f, -0.5f, 0.5f,    1.0f, 0.0f,// lower-right corner
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,// top-right corner
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f,  0.5f, 0.5f,    0.0f, 1.0f,// top-left corner
            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,// lower-left corner
            // Back Face
            0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
            -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,// lower-right corner
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
            0.5f,  0.5f, -0.5f,    0.0f, 1.0f,// top-left corner
            0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
            // Left Face
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
            -0.5f, -0.5f,  0.5f,    1.0f, 0.0f,// lower-right corner
            -0.5f,  0.5f,  0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f,  0.5f,  0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,// top-left corner
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
            // Bottom Face
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
             0.5f, -0.5f, -0.5f,    1.0f, 0.0f,// lower-right corner
             0.5f, -0.5f,  0.5f,    1.0f, 1.0f,// top-right corner
             0.5f, -0.5f,  0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f, -0.5f,  0.5f,    0.0f, 1.0f,// top-left corner
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,// lower-left corner
            // Right Face
            0.5f, -0.5f,  0.5f,    0.0f, 0.0f,// lower-left corner
            0.5f, -0.5f, -0.5f,    1.0f, 0.0f,// lower-right corner
            0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
            0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
            0.5f,  0.5f,  0.5f,    0.0f, 1.0f,// top-left corner
            0.5f, -0.5f,  0.5f,    0.0f, 0.0f,// lower-left corner
            // Top Face
            -0.5f,  0.5f, 0.5f,    0.0f, 0.0f,// lower-left corner
             0.5f,  0.5f, 0.5f,    1.0f, 0.0f,// lower-right corner
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,// top-right corner
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,// top-left corner
            -0.5f,  0.5f, 0.5f,    0.0f, 0.0f,// lower-left corner
        };

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); 

    stbi_set_flip_vertically_on_load(true);

	unsigned int texture;
	glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load and Generate the Texture
	unsigned char *data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
	if (data){
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	    glGenerateMipmap(GL_TEXTURE_2D);
	}

	else{
	    std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

    float rotation=0.0f;
    double prevTime=glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        ImGui::Render();

        glUseProgram(shaderProgram);

        double curTime=glfwGetTime();
        if(curTime-prevTime>=1/60){
            rotation+=2.0f;
            prevTime=curTime;
        }

        glm::mat4 model = glm::mat4(1.0f);

        // For Triangle - Comment this line 
        model=glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        // For Cube - Uncomment this line 
        model=glm::rotate(model, glm::radians(rotation), glm::vec3(1.0f, 1.0f, 1.0f));
        proj=glm::perspective(glm::radians(45.0f), (float)(1000/1000), 0.1f, 100.0f);
        

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projLoc = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}