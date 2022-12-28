#pragma once

#include <glm/glm.hpp>

class Renderer {
public:
	Renderer();
	~Renderer();

	void BatchBegin();
	void BatchEnd();
	void Flush();

	void CreateQuad(glm::vec2& coords, glm::vec2& size, float layer, glm::vec4& color);
	void CreateQuad(glm::vec2& coords, glm::vec2& size, float layer, uint32_t textureID);
private:

};