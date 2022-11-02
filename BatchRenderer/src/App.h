#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>

#include "Shader.h"

class App
{
public:
	App(const std::string title = "Title", uint32_t width = 1280, uint32_t height = 720);
	~App();

	int Run();
private:
	float m_ClearColor = 0.0f;
	GLFWwindow* m_Window;

	std::string m_Title;
	const uint32_t m_Width, m_Height;

	glm::mat4 m_Proj;

	std::unique_ptr<Shader> m_Shader;

	GLuint m_QuadVA, m_QuadVB, m_QuadIB;

	GLuint m_Pic1, m_Pic2;

	void OnUpdate();
	void OnRender();
	void OnImGuiRender();

	float m_FirstQuad[2] = { 500.0f, 500.0f };
};

