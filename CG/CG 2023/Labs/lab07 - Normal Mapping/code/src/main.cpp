#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include "utils.h"

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define  GLM_FORCE_RADIANS
#define  GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

//Globals
int screen_width = 1000, screen_height=1000;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
GLint lpos_world_uniform, eye_normal_uniform;
glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations

double oldX, oldY, currentX, currentY;
bool isDragging=false;

void rasterizer();
void createMeshObject(unsigned int &, unsigned int &);

void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &);
glm::vec3 getTrackBallVector(double x, double y);
glm::vec3 norm(glm::vec3 a, glm::vec3 b, glm::vec3 c);

GLuint nVertices;
unsigned int texture, normalMap;
int width, height, nrChannels;

int main(int, char**)
{   
    rasterizer(); // Render Mesh
    return 0;
}

void rasterizer(){
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    
    //Get handle to light position variable in shader
    lpos_world_uniform = glGetUniformLocation(shaderProgram, "lpos_world");
    if(lpos_world_uniform == -1){
        fprintf(stderr, "Could not bind location: lpos_world\n");
    }

    //Get handle to eye normal variable in shader
    eye_normal_uniform = glGetUniformLocation(shaderProgram, "eye_normal");
    if(eye_normal_uniform == -1){
        fprintf(stderr, "Could not bind location: eye_normal. Specular Lighting Switched Off.\n");
    }

    glUseProgram(shaderProgram);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram);

    createMeshObject(shaderProgram, VAO);

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
            glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
            modelT = glm::rotate(modelT, angle, axis_in_object_coord);
            glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

            oldX = currentX;
            oldY = currentY;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO); 

        glUniform3f(lpos_world_uniform, -50.0, 500.0, 30.0);
        glUniform3f(eye_normal_uniform, 40.0, -40.0, 40.0);
        
        glDrawArrays(GL_TRIANGLES, 0, nVertices);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    cleanup(window);
}

void setupModelTransformation(unsigned int &program)
{
    //Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

    //Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    vModel_uniform = glGetUniformLocation(program, "vModel");
    if(vModel_uniform == -1){
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}


void setupViewTransformation(unsigned int &program)
{
    //Viewing transformations (World -> Camera coordinates
    //Camera at (40, 20, 40)  in a right handed coordinate system
    viewT = glm::lookAt(glm::vec3(40.0, 20.0, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    //Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    vView_uniform = glGetUniformLocation(program, "vView");
    if(vView_uniform == -1){
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program)
{
    //Projection transformation
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);

    //Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if(vProjection_uniform == -1){
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
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

glm::vec3 norm(glm::vec3 a, glm::vec3 b){
    glm::vec3 normal = cross(a, b);
    return normalize(normal);
}

void computeTangentAndBitangent(
    GLfloat pos1x, GLfloat pos1y, GLfloat pos1z,
    GLfloat pos2x, GLfloat pos2y, GLfloat pos2z,
    GLfloat pos3x, GLfloat pos3y, GLfloat pos3z,
    GLfloat uv1x, GLfloat uv1y,
    GLfloat uv2x, GLfloat uv2y,
    GLfloat uv3x, GLfloat uv3y,
    glm::vec3 &tangent,
    glm::vec3 &bitangent
) {
    // Create vectors for the edges of the triangle in both position and UV space
    glm::vec3 edge1(pos2x - pos1x, pos2y - pos1y, pos2z - pos1z);
    glm::vec3 edge2(pos3x - pos2x, pos3y - pos2y, pos3z - pos2z);

    glm::vec2 deltaUV1(uv2x - uv1x, uv2y - uv1y);
    glm::vec2 deltaUV2(uv3x - uv2x, uv3y - uv2y);

    // Compute the tangent and bitangent
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);
}

void createMeshObject(unsigned int &program, unsigned int &shape_VAO){
    vector<int> vertex_indices, uv_indices, normal_indices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    int ct = 0;

    FILE * file = fopen("src/bunny.obj", "r");
    if( file == NULL ) printf("File not found\n");

    float scale = 0.05;

    while(true){

        char head[128];
        int res = fscanf(file, "%s", head);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( head, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            ct++;
        }

        else if ( strcmp( head, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }

        else if ( strcmp( head, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }

        else if ( strcmp( head, "f" ) == 0 ){
            string vertex1, vertex2, vertex3;
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("OBJ File may not contain texture coordinates or normal coordinates\n");
            }
            vertex_indices.push_back(vertexIndex[0]);
            vertex_indices.push_back(vertexIndex[1]);
            vertex_indices.push_back(vertexIndex[2]);
            uv_indices.push_back(uvIndex[0]);
            uv_indices.push_back(uvIndex[1]);
            uv_indices.push_back(uvIndex[2]);
            normal_indices.push_back(normalIndex[0]);
            normal_indices.push_back(normalIndex[1]);
            normal_indices.push_back(normalIndex[2]);
        }
    }
    fclose(file);

    glUseProgram(program);

    //Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }

    int vNormal_attrib = glGetAttribLocation(program, "vertex_norm");
    if(vNormal_attrib == -1) {
        std::cout << "Could not bind location: vertex_norm\n" ;
    }else{
        std::cout << "aNormal found at location " << vNormal_attrib << std::endl;
    }

    int vTexture_attrib = glGetAttribLocation(program, "aTexCoord");
    if(vTexture_attrib == -1) {
        std::cout << "Could not bind location: aTexCoord\n" ;
    }else{
        std::cout << "aTexCoord found at location " << vTexture_attrib << std::endl;
    }

    int vTangent_attrib = glGetAttribLocation(program, "vTangent");
    if(vTangent_attrib == -1) {
        std::cout << "Could not bind location: vTangent\n" ;
    }else{
        std::cout << "vTangent found at location " << vTangent_attrib << std::endl;
    }

    int vBitangent_attrib = glGetAttribLocation(program, "vBitangent");
    if(vBitangent_attrib == -1) {
        std::cout << "Could not bind location: vBitangent\n" ;
    }else{
        std::cout << "vBitangent found at location " << vBitangent_attrib << std::endl;
    }

    GLfloat *shape_vertices = new GLfloat[vertex_indices.size()*3];

    nVertices = vertex_indices.size()*3;

    for(int i=0; i<vertex_indices.size(); i++){
        int vertexIndex = vertex_indices[i];
        shape_vertices[i*3] = temp_vertices[vertexIndex-1][0]*scale;
        shape_vertices[i*3+1] = temp_vertices[vertexIndex-1][1]*scale;
        shape_vertices[i*3+2] = temp_vertices[vertexIndex-1][2]*scale;
    }

    GLfloat *vertex_textures = new GLfloat[vertex_indices.size()*2];

    for(int i=0; i<uv_indices.size(); i++ ){
        vertex_textures[i*2] = temp_uvs[uv_indices[i]-1][0];
        vertex_textures[i*2+1] = temp_uvs[uv_indices[i]-1][1];
    }

    GLfloat *vertex_normals = new GLfloat[normal_indices.size()*3];
    GLfloat *vertex_tangents = new GLfloat[vertex_indices.size()*3];
    GLfloat *vertex_bitangents = new GLfloat[vertex_indices.size()*3];

    //generated normals for the triangle mesh
    for (int i = 0; i < vertex_indices.size(); i += 3) {
        glm::vec3 v1 = glm::vec3(shape_vertices[(i+1)*3]-shape_vertices[i*3], shape_vertices[(i+1)*3+1]-shape_vertices[i*3+1],shape_vertices[(i+1)*3+2]-shape_vertices[i*3+2]);
        glm::vec3 v2 = glm::vec3(shape_vertices[(i+2)*3]-shape_vertices[(i+1)*3], shape_vertices[(i+2)*3+1]-shape_vertices[(i+1)*3+1],shape_vertices[(i+2)*3+2]-shape_vertices[(i+1)*3+2]);

        glm::vec3 n = norm(v1, v2);
        
        vertex_normals[i*3] = n.x;
        vertex_normals[(i+1)*3] = n.x;
        vertex_normals[(i+2)*3] = n.x;
        vertex_normals[i*3+1] = n.y;
        vertex_normals[(i+1)*3+1] = n.y;
        vertex_normals[(i+2)*3+1] = n.y;
        vertex_normals[i*3+2] = n.z;
        vertex_normals[(i+1)*3+2] = n.z;
        vertex_normals[(i+2)*3+2] = n.z;

        glm::vec3 tangent, bitangent;

        computeTangentAndBitangent(
            shape_vertices[i*3], shape_vertices[i*3+1], shape_vertices[i*3+2], // current vertex
            shape_vertices[(i+1)*3], shape_vertices[(i+1)*3+1], shape_vertices[(i+1)*3+2], // next vertex
            shape_vertices[(i+2)*3], shape_vertices[(i+2)*3+1], shape_vertices[(i+2)*3+2], // next of next vertex
            vertex_textures[i*2], vertex_textures[i*2+1], // current uv
            vertex_textures[(i+1)*2], vertex_textures[(i+1)*2+1], // next uv
            vertex_textures[(i+2)*2], vertex_textures[(i+2)*2+1], // next of next uv
            tangent, bitangent
        );

        for (int j = 0; j < 3; j++) {
            vertex_tangents[(i+j)*3] = tangent.x;
            vertex_tangents[(i+j)*3+1] = tangent.y;
            vertex_tangents[(i+j)*3+2] = tangent.z;

            vertex_bitangents[(i+j)*3] = bitangent.x;
            vertex_bitangents[(i+j)*3+1] = bitangent.y;
            vertex_bitangents[(i+j)*3+2] = bitangent.z;
        }
    }

    //Generate VAO object
    glGenVertexArrays(1, &shape_VAO);
    glBindVertexArray(shape_VAO);

    //Create VBOs for the VAO
    GLuint vertex_VBO; // Vertex Buffer
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, shape_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 00, 0);
    delete []shape_vertices;

    GLuint normal_VBO; // Normal Buffer
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, vertex_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vNormal_attrib);
    glVertexAttribPointer(vNormal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []vertex_normals;

    GLuint texture_VBO;
    glGenBuffers(1,  &texture_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, texture_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*2, vertex_textures, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vTexture_attrib);
    glVertexAttribPointer(vTexture_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    delete [] vertex_textures;

    // Create VBO for tangents
    GLuint tangent_VBO;
    glGenBuffers(1, &tangent_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, tangent_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, vertex_tangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vTangent_attrib);
    glVertexAttribPointer(vTangent_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete [] vertex_tangents;

    // Create VBO for bitangents
    GLuint bitangent_VBO;
    glGenBuffers(1, &bitangent_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bitangent_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, vertex_bitangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vBitangent_attrib);
    glVertexAttribPointer(vBitangent_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete [] vertex_bitangents;

    // Set the uniform locations for the shaders
    int textureLocation = glGetUniformLocation(program, "texture1");
    int normalMapLocation = glGetUniformLocation(program, "normalMap");

    // Set uniforms to correspond to the GL_TEXTURE0 and GL_TEXTURE1 units
    glUniform1i(textureLocation, 0); // Texture unit 0 is for the diffuse texture
    glUniform1i(normalMapLocation, 1); // Texture unit 1 is for the normal map

    // Load the diffuse texture
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, texture);

    // Flip the texture on load to match OpenGL's bottom-left origin
    stbi_set_flip_vertically_on_load(true);

    // Load the image data for the diffuse texture
    unsigned char *data = stbi_load("./texture/texture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the diffuse texture
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // Free the image memory

    // Set texture parameters for the diffuse texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the normal map
    glGenTextures(1, &normalMap);
    glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // Flip the texture on load to match OpenGL's bottom-left origin
    stbi_set_flip_vertically_on_load(true);

    // Load the image data for the normal map
    unsigned char *data2 = stbi_load("./texture/normal.jpg", &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the normal map
    }
    else
    {
        std::cout << "Failed to load normal map" << std::endl;
    }
    stbi_image_free(data2); // Free the image memory

    // Set texture parameters for the normal map
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the buffers to prevent accidental changes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
