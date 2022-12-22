#include "App.h"

#include <iostream>
#include <array>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

const size_t MaxQuadCount = 50;
const size_t MaxVertexCount = MaxQuadCount * 4;
const size_t MaxIndexCount = MaxQuadCount * 6;

static Vertex* CreateQuad(Vertex* target, 
    glm::vec2 coords, glm::vec2 size, float layer, float textureID) {
    layer = layer / 100.0f;
    
    target->Position = { coords.x, coords.y, layer };
    target->Color = { 0.96f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 0.0f, 0.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { coords.x + size.x, coords.y, layer };
    target->Color = { 0.16f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 1.0f, 0.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { coords.x + size.x, coords.y + size.y, layer };
    target->Color = { 0.16f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 1.0f, 1.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { coords.x, coords.y + size.y, layer };
    target->Color = { 0.96f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 0.0f, 1.0f };
    target->TexID = textureID;
    ++target;

    return target;
}

static Vertex* CreateQuad(Vertex* target, int x, int y, int sizex, int sizey, int layer, int textureID) {
    return CreateQuad(target, {(float)x, (float)y}, {(float)sizex, (float)sizey}, (float)layer, (float)textureID);
}

App::App(const std::string title, uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height), m_Title(title)
{
    setlocale(LC_ALL, "");

    if (!glfwInit()) {
        std::cerr << "glfw is not OK";
        exit(1);
    }

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
    if (!m_Window) {
        std::cerr << "window is not OK";
        exit(1);
    }

    glfwMakeContextCurrent(m_Window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "glew is not OK";
        exit(1);
    }

    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Shader = std::make_unique<Shader>(
        "assets/shaders/test.vert.glsl",
        "assets/shaders/test.frag.glsl"
        );

    glUseProgram(m_Shader->GetRendererID());
    auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
    int samplers[3] = { 0,1,2 };
    glUniform1iv(loc, 3, samplers);

    glCreateVertexArrays(1, &m_QuadVA);
    glBindVertexArray(m_QuadVA);

    glCreateBuffers(1, &m_QuadVB);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;
    for (size_t i = 0; i < MaxIndexCount; i += 6) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glCreateBuffers(1, &m_QuadIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_Proj = glm::ortho(0.0f, (float)m_Width, 0.0f, (float)m_Height, -1.0f, 1.0f);

    m_Pic1 = std::make_unique<Texture>("assets/masyunya.png");
    m_Pic2 = std::make_unique<Texture>("assets/neverhood.png");
    m_Pic3 = std::make_unique<Texture>("assets/background.jpg");

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

int App::Run()
{
    while (!glfwWindowShouldClose(m_Window)) {
        OnUpdate();
        OnRender();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        OnImGuiRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
    return 0;
}

void App::OnUpdate()
{

}

void App::OnRender()
{
    GLsizei indexCount = 0;
    SetBackground(m_BackColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_BackgroundPic[0] = {};
    m_BackgroundPic[1] = {};
    m_BackgroundPic[2] = {};
    m_BackgroundPic[3] = {};
    std::array<Vertex, 200> vertices;
    Vertex* buffer = vertices.data();

    if (m_SetBackground) {
        SetBackground(*m_Pic2, m_StretchBackground);
        memcpy(buffer, m_BackgroundPic, sizeof(m_BackgroundPic));
        buffer += 4;
        indexCount += 6;
    }
    /*
    for (float y = 0; y < 5; ++y) {
        for (float x = 0; x < 9; ++x) {
            buffer = CreateQuad(buffer, { x * 100,y * 100 }, { 100, 100 }, 1.0f,1.0f);
            indexCount += 6;
        }
    }*/

    buffer = CreateQuad(buffer, { m_FirstQuad.x, m_FirstQuad.y }, { 200, 200 },2.0f, 0.0f);
    indexCount += 6;

    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

    glUseProgram(m_Shader->GetRendererID());

    glBindTextureUnit(0, m_Pic1->PictureDescriptor);
    glBindTextureUnit(1, m_Pic2->PictureDescriptor);
    glBindTextureUnit(2, m_Pic3->PictureDescriptor);

    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_Proj));

    glBindVertexArray(m_QuadVA);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void App::OnImGuiRender()
{
    ImGui::ColorEdit4("Background Color", &m_BackColor.r);
    ImGui::DragFloat2("Pisunya position", &m_FirstQuad.x, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("Turn ON Background Pic", &m_SetBackground);
    ImGui::Checkbox("Stretch Background Pic", &m_StretchBackground);
}

void App::SetBackground(const Texture& pic, bool fullscreen)
{
    if (fullscreen) {
        CreateQuad(m_BackgroundPic, 
            0.0f,0.0f,
            (float)m_Width, (float)m_Height,
            0.0f ,(float)pic.PictureDescriptor -1.0f);
    }
    else {
        float picWidth = pic.GetWidth();
        float picHeight = pic.GetHeight();
        CreateQuad(m_BackgroundPic, 
            m_Width/2 - picWidth/2, m_Height/2-picHeight/2,
            picWidth, picHeight,
            0.0f, (float)pic.PictureDescriptor - 1);
    }
}

void App::SetBackground(glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}
