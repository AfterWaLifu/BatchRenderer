#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class Shader
{
private:
	uint32_t m_RendererID;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	~Shader();

	uint32_t GetRendererID() { return m_RendererID; }

private:
	std::string ReadFile(const std::string& filename);
	unsigned int CompileShader(unsigned int type, const std::string& code);
	unsigned int CreateShader(const std::string& m_VertShaderFilename, const std::string& m_FragShaderFilename);
};
