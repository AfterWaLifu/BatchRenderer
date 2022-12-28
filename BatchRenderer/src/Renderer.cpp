#include "Renderer.h"

const size_t MaxQuadCount = 50;
const size_t MaxVertexCount = MaxQuadCount * 4;
const size_t MaxIndexCount = MaxQuadCount * 6;
const size_t MaxTextures = 32;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::BatchBegin()
{
}

void Renderer::BatchEnd()
{
}

void Renderer::Flush()
{
}

void Renderer::CreateQuad(glm::vec2& coords, glm::vec2& size, float layer, glm::vec4& color)
{
}

void Renderer::CreateQuad(glm::vec2& coords, glm::vec2& size, float layer, uint32_t textureID)
{
}
