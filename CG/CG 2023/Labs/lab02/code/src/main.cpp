#include "utils/utils.hpp"
#include "drawable.hpp"
#include "camera/camera.hpp"
#include "mesh/mesh.hpp"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


Mesh *testCubeMesh()
{
	GLfloat verts[] = {0, 0, 0,
                                1, 0, 0,
                                1, 1, 0,
                                0, 1, 0,
                                0, 0, 1,
                                1, 0, 1,
                                1, 1, 1,
                                0, 1, 1
                                };
	
    GLuint indices[] = {0, 1, 2,
                                0, 2, 3,
                                4, 5, 6,
                                4, 6, 7,
                                0, 4, 7,
                                0, 7, 3,
                                1, 5, 6,
                                1, 6, 2,
                                0, 1, 5,
                                0, 5, 4,
                                3, 2, 6,
                                3, 6, 7
                                };

	return new Mesh(verts,indices,8,12);
	// Mesh(vertex, indices, nV, nF)
	// 		nV x 3, nF x 3
}

Mesh *testSphereMesh()
{
	std::vector<GLfloat> verts;
	std::vector<GLuint> indices;

	int n = 100;
	float r = 0.5f;
	float pi = 3.14159265358979323846f;
	float theta, phi;
	float x, y, z;
	float dTheta = 2 * pi / n;
	float dPhi = pi / n;

	for (int i = 0; i <= n; i++)
	{
		theta = i * dTheta;
		for (int j = 0; j <= n; j++)
		{
			phi = j * dPhi;
			x = r * sin(phi) * cos(theta);
			y = r * sin(phi) * sin(theta);
			z = r * cos(phi);
			verts.push_back(x);
			verts.push_back(y);
			verts.push_back(z);
		}
	}

	for (int i = 0; i <= n; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			int a = i * (n + 1) + j;
			int b = a + 1;
			int c = (i + 1) * (n + 1) + j;
			int d = c + 1;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(c);
			indices.push_back(b);
			indices.push_back(d);
		}
	}

	return new Mesh(verts.data(), indices.data(), verts.size() / 3, indices.size() / 3);
}
int main(int, char **)
{
	// Setup window
	GLFWwindow *window = setupWindow(SCREEN_W, SCREEN_H, "Parametric Representations of Surfaces");
	ImGuiIO &io = ImGui::GetIO(); // Create IO

	double last_time = 0;
	double delta_time = 0;

	unsigned int shader_program = createProgram("shaders/vshader.vs", "shaders/fshader.fs");

	Camera *cam = new Camera(glm::vec3(-5.0f, 3.0f, 3.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
							 45.0f, 0.1f, 10000.0f, window);

	cam->setProjectionTransformation(shader_program);
	cam->setViewTransformation(shader_program);

	Mesh *mesh = testSphereMesh();
	//Render a sphere using parametric representation.
	// ParametricMesh *mesh = new ParametricMesh(100);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		double curr_time = static_cast<double>(glfwGetTime());
		delta_time = curr_time - last_time;
		last_time = curr_time;

		if (!io.WantCaptureMouse)
		{
			cam->process_input(window, delta_time);
			cam->setViewTransformation(shader_program);
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(WHITE.x, WHITE.y, WHITE.z, WHITE.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh->draw(shader_program);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	cleanup(window);

	return 0;
}
