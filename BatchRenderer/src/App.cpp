#include "App.h"

#include <iostream>
#include <array>

#include "3pty/imgui/imgui.h"
#include "3pty/imgui/imgui_impl_glfw.h"
#include "3pty/imgui/imgui_impl_opengl3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

struct Vec2 {
    float x, y;
};
struct Vec3 {
    float x, y, z;
};
struct Vec4 {
    float x, y, z, w;
};

struct Vertex {
    Vec3 Position;
    Vec4 Color;
    Vec2 TexCoords;
    float TexID;
};

static GLuint LoadTexture(const std::string& path)
{
    int w, h, bits;

    stbi_set_flip_vertically_on_load(1);
    auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    if (pixels) {
        stbi_image_free(pixels);
    }

    return textureID;
}

static Vertex* CreateQuad(Vertex* target, 
    float x, float y, float sizex, float sizey, float textureID) {

    target->Position = { x, y, 0.0f };
    target->Color = { 0.96f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 0.0f, 0.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { x + sizex, y, 0.0f };
    target->Color = { 0.16f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 1.0f, 0.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { x + sizex, y + sizey, 0.0f };
    target->Color = { 0.16f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 1.0f, 1.0f };
    target->TexID = textureID;
    ++target;

    target->Position = { x, y + sizey, 0.0f };
    target->Color = { 0.96f, 0.6f, 0.18f, 1.0f };
    target->TexCoords = { 0.0f, 1.0f };
    target->TexID = textureID;
    ++target;

    return target;
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
    int samplers[2] = { 0,1 };
    glUniform1iv(loc, 2, samplers);

    const size_t MaxQuadCount = 1000;
    const size_t MaxVertexCount = MaxQuadCount * 4;
    const size_t MaxIndexCount = MaxQuadCount * 6;

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

    m_Pic1 = LoadTexture("assets/masyunya.png");
    m_Pic2 = LoadTexture("assets/neverhood.png");

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
    glClearColor(m_ClearColor, m_ClearColor, m_ClearColor, m_ClearColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uint32_t indexCount = 0;

    std::array<Vertex, 1000> vertices;
    Vertex* buffer = vertices.data();
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            buffer = CreateQuad(buffer, (float)x*100, (float)y*100, 100, 100, (float)((x + y) % 2));
            indexCount += 6;
        }
    }

    buffer = CreateQuad(buffer, m_FirstQuad[0], m_FirstQuad[1], 100, 100, 0.0f);
    indexCount += 6;

    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

    glUseProgram(m_Shader->GetRendererID());

    glBindTextureUnit(0, m_Pic1);
    glBindTextureUnit(1, m_Pic2);

    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_Proj));

    glBindVertexArray(m_QuadVA);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void App::OnImGuiRender()
{
    ImGui::DragFloat("Background Color", &m_ClearColor, 0.02, 0.0f, 1.0f);
    ImGui::DragFloat2("First pos", m_FirstQuad, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}