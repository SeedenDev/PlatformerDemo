#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>

#include "ApplicationWindow.h"
#include "Input.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/Texture.h"
#include "Scene/Camera.h"
#include "Scene/Quad.h"
#include "Scene/Level.h"

int main(void)
{
    std::srand(time(NULL));
    ApplicationWindow appWindow("Platformer Demo", 1080, 720);

    std::cout << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Level defaultLevel("assets/textures/level0.png");

    Texture terrainTexture("assets/textures/terrain.png");
    Texture playerTexture("assets/textures/player.png");
    Texture enemyTexture("assets/textures/enemy.png");
    Texture flagTexture("assets/textures/flag.png");
    Texture coinTexture("assets/textures/coin.png");
    terrainTexture.Bind(0);
    playerTexture.Bind(1);
    enemyTexture.Bind(2);
    flagTexture.Bind(3);
    coinTexture.Bind(4);

    Shader defaultShader("assets/shaders/basic2DQuad.vert", "assets/shaders/basic2DQuad.frag");

    Camera camera(appWindow.GetWindowPointer(), appWindow.GetAspectRatio());

    glm::vec3 clearColor(0.0f);
    int tab = 0;
    int selectedItem = -1;

    double lastTime = glfwGetTime();
    // Graphics settings for fps: [SET/UNLIMITED/VSYNC]
    float fpsLimit = 60.0;
    double deltaTimeLimit = 1.0 / fpsLimit;
    bool unlimitedFPS = 0;
    if (unlimitedFPS || fpsLimit != 60.0)
        appWindow.ToggleVsync(); // Toggle off V-Sync

    // Average fps
    int averageFps = 0;
    const size_t avgBufferSize = 50;
    unsigned int avgBufferPtr = 0;
    float avgBuffer[avgBufferSize] = { 0 };

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!appWindow.ShouldClose())
    {
        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetWindowAttrib(appWindow.GetWindowPointer(), GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        /* DeltaTime into [Vsync/Set/Unlimited]-FPS based render */
        bool vSync = appWindow.IsVsync();
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (vSync || (!vSync && (!unlimitedFPS && deltaTime >= deltaTimeLimit) || (unlimitedFPS)))
        {
            /* Fps counter */
            float fps = 1 / deltaTime;
            double msPerFrame = 1000.0 / fps;

            avgBuffer[avgBufferPtr++] = fps;
            if (avgBufferPtr >= avgBufferSize) avgBufferPtr = 0;
            averageFps = 0;
            for (float v : avgBuffer) averageFps += v;
            averageFps /= avgBufferSize; /*if using msPerFrame: averageFps = 1000 * avgBufferSize / averageFps;*/

            std::string windowTitle =
                "Platformer Demo (FPS: " + std::to_string(fps) + " (avg:" + std::to_string(averageFps) + ") - " + std::to_string(msPerFrame) + "ms) DeltaTime:" + std::to_string(deltaTime);

            appWindow.SetTitle(windowTitle);
            lastTime = currentTime;

            /* Update */
            if (Input::IsKeyPressed(GLFW_KEY_R))
            {
                defaultShader.Reload();
            }
            appWindow.Update();

            defaultLevel.runLogic(deltaTime);
            camera.Update(defaultLevel.player.x, defaultLevel.player.y, defaultLevel.levelWidth);
            camera.SetAspectRatio(appWindow.GetAspectRatio());

            glm::mat4 view = camera.GetView();
            glm::mat4 projection = camera.GetProj();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            //rendering level
            defaultShader.Bind();
            defaultShader.SetUniform2f("u_PlayerPos", defaultLevel.player.x, defaultLevel.player.y);
            // render player
            defaultShader.SetUniform1i("u_Texture", playerTexture.GetSlot());
            Quad& playerQuad = defaultLevel.player.quad;
            glm::mat4 model = playerQuad.GetModelMatrix();
            glm::mat4 MVP = projection * view * model;
            defaultShader.SetUniformMat4f("u_Model", model);
            defaultShader.SetUniformMat4f("u_MVP", MVP);
            playerQuad.Draw();
            // render tiles
            defaultShader.SetUniform1i("u_Texture", terrainTexture.GetSlot());
            for (const Tile& tile : defaultLevel.tiles)
            {
                if (!tile.visible) continue;
                model = tile.quad.get()->GetModelMatrix();
                MVP = projection * view * model;
                defaultShader.SetUniformMat4f("u_Model", model);
                defaultShader.SetUniformMat4f("u_MVP", MVP);
                tile.quad.get()->Draw();
            }
            // render enemies
            defaultShader.SetUniform1i("u_Texture", enemyTexture.GetSlot());
            for (const Enemy& enemy : defaultLevel.enemies)
            {
                if (!enemy.alive) continue;
                model = enemy.quad.get()->GetModelMatrix();
                MVP = projection * view * model;
                defaultShader.SetUniformMat4f("u_Model", model);
                defaultShader.SetUniformMat4f("u_MVP", MVP);
                enemy.quad.get()->Draw();
            }
            defaultShader.Unbind();
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::BeginTabBar("Info");
                const glm::vec3& camPos = camera.GetPosition();
                ImGui::Text("Camera:");
                ImGui::Text("%.2f;%.2f;%.2f (%.2f;%.2f)", camPos.x, camPos.y, camPos.z, camera.GetYaw(), camera.GetPitch());
                ImGui::Text("Zoom: %.1f", camera.GetZoomLevelX());
                float imguiFps = ImGui::GetIO().Framerate;
                ImGui::Text("Avg: %.3f ms/frame (%.1f FPS)", 1000.0 / imguiFps, imguiFps);
                ImGui::EndTabBar();
                ImGui::Begin("Settings");
                if (ImGui::Checkbox("VSync", &vSync)) appWindow.ToggleVsync();
                if (!vSync)
                {
                    ImGui::Checkbox("Unlimited FPS", &unlimitedFPS);
                    if(!unlimitedFPS)
                    {
                        ImGui::SameLine();
                        if(ImGui::SliderFloat("FPS limit", &fpsLimit, 0.0, 200.0)) deltaTimeLimit = 1.0 / fpsLimit;
                    }
                }
                if (ImGui::ColorEdit3("ClearColor", &clearColor[0])) glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(appWindow.GetWindowPointer());
        }
    }
}