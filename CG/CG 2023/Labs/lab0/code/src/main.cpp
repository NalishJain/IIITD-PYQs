/**
 * @file main.cpp
 * @brief  This code renders an ImGui widget along with an OpenGL background.

 * @author Vishwesh Vhavle
 * @date August 1, 2023
 */

 #include "utils.h"

int main()
{
    // Setup window
    GLFWwindow *window = setupWindow(1920, 1080); // (Width, Height) PLAY AROUND WITH THIS
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clear_color = ImColor(114, 144, 154); // PLAY AROUND WITH THIS
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            // ImGui widget example
            ImGui::Begin("Widget Window"); // PLAY AROUND WITH THIS
            ImGui::Text("Welcome to ImGui with OpenGL!"); // PLAY AROUND WITH THIS

            ImGui::ColorEdit3("Clear color", (float *)&clear_color); // PLAY AROUND WITH THIS
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate); // PLAY AROUND WITH THIS
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        // glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // PLAY AROUND WITH THIS
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup ImGui and GLFW
    cleanup(window);

    return 0;
}