#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>

#include "Shader.h"
#include "Texture.h"

#include "Structs.h"

class App
{
public:
	App(const std::string title = "Title", uint32_t width = 1280, uint32_t height = 720);
	~App();

	int Run();
private:
	GLFWwindow* m_Window;

	std::string m_Title;
	const uint32_t m_Width, m_Height;

	glm::mat4 m_Proj;

	std::unique_ptr<Shader> m_Shader;

	GLuint m_QuadVA, m_QuadVB, m_QuadIB;

	std::unique_ptr<Texture> m_Pic1, m_Pic2, m_Pic3;

	glm::vec2 m_FirstQuad = { 505.0f, 300.0f };
	glm::vec4 m_BackColor = { 0.0f, 0.0f ,0.0f, 0.0f };
	bool m_SetBackground = false;
	bool m_StretchBackground = false;
	Vertex m_BackgroundPic[4];

	void OnUpdate();
	void OnRender();
	void OnImGuiRender();
	
	void SetBackground(const Texture& pic, bool fullscreen = true);
	void SetBackground(glm::vec4 color);
};