/*
 * @file main.cpp
 * @brief This code loads the .OBJ mesh, passes vertices, normals, 
 * eye normal and light position to shader 
 *
 * @author Vishwesh Vhavle
 * @date September 20, 2023
 */

#include "utils.h"

//Globals
int screen_width = 800, screen_height = 800;
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

GLuint nVertices;
float scale = 1; // Change Scale of the model as needed

int main()
{   
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object
    ImVec4 clearColor = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/gourad_vshader.vs", "./shaders/gourad_fshader.fs");
    // unsigned int shaderProgram = createProgram("./shaders/phong_vshader.vs", "./shaders/phong_fshader.fs");
    
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
    int mode = 1; // 0 for manual drag, 1 for auto rotate

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        double current_seconds = glfwGetTime();

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
        float angle = 0.0f;
        prevLeftButtonState = leftButtonState;
        if (mode == 0) {
            if (isDragging && (currentX != oldX || currentY != oldY)) {
                // Drag rotation
                glm::vec3 va = getTrackBallVector(oldX, oldY);
                glm::vec3 vb = getTrackBallVector(currentX, currentY);

                angle = acos(std::min(1.0f, glm::dot(va,vb)));
                glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
                glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
                glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
                modelT = glm::rotate(modelT, angle, axis_in_object_coord);
                glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

                oldX = currentX;
                oldY = currentY;
            }
        } else if (mode == 1) {
            // Autorotation
            angle += 0.05;
            modelT = glm::rotate(modelT, angle, glm::vec3(0.0, 1.0, 0.0));
            glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
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

void createMeshObject(unsigned int &program, unsigned int &shape_VAO){
    vector<int> vertex_indices, uv_indices, normal_indices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    int ct = 0;

    scale = 0.05; // Change Scale of the model as needed
    FILE * file = fopen("src/bunny.obj", "r");
    if( file == NULL ) printf("File not found\n");

    while(true){

        char head[128];
        // read the first word of the line
        int res = fscanf(file, "%s", head);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( head, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            // printf("%f %f %f\n", vertex.x, vertex.y, vertex.z );
            temp_vertices.push_back(vertex);
            ct++;
        }

        else if ( strcmp( head, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            // printf("%f %f\n", uv.x, uv.y );
            temp_uvs.push_back(uv);
        }

        else if ( strcmp( head, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            // printf("%f %f %f\n", normal.x, normal.y, normal.z );
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

    int vNormal_attrib = glGetAttribLocation(program, "vNormal");
    if(vNormal_attrib == -1) {
        std::cout << "Could not bind location: vNormal\n" ;
    }

    GLfloat *shape_vertices = new GLfloat[vertex_indices.size()*3];
    GLfloat *vertex_normals = new GLfloat[normal_indices.size()*3];

    nVertices = vertex_indices.size()*3;

    for(int i=0; i<vertex_indices.size(); i++){
        int vertexIndex = vertex_indices[i];
        shape_vertices[i*3] = temp_vertices[vertexIndex-1][0]*scale;
        shape_vertices[i*3+1] = temp_vertices[vertexIndex-1][1]*scale;
        shape_vertices[i*3+2] = temp_vertices[vertexIndex-1][2]*scale;
    }
  
    //generated normals for the triangle mesh
    for(int i=0;i<vertex_indices.size();i+=3){
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
    }

    for(int i=0;i<vertex_indices.size();i+=3){

        glm::vec3 a = glm::vec3(shape_vertices[i*3], shape_vertices[i*3+1], shape_vertices[i*3+2]);
        glm::vec3 b = glm::vec3(shape_vertices[(i+1)*3], shape_vertices[(i+1)*3+1], shape_vertices[(i+1)*3+2]);
        glm::vec3 c = glm::vec3(shape_vertices[(i+2)*3], shape_vertices[(i+2)*3+1], shape_vertices[(i+2)*3+2]);
        glm::vec3 n = glm::vec3(vertex_normals[i*3], vertex_normals[i*3+1], vertex_normals[i*3+2]);
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

  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
}
