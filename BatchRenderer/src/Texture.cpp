#include "Texture.h"

#include <GL/glew.h>
#include <stb_image.h>

Texture::Texture(const std::string& path)
	:PictureDescriptor(0),
	m_Width(0), m_Height(0), m_BPP(0),
	m_Data(nullptr), m_DataSize(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	glGenTextures(1, &PictureDescriptor);
	glBindTexture(GL_TEXTURE_2D, PictureDescriptor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_DataSize = m_Width * m_Height * m_BPP;
}

Texture::~Texture()
{
	if (m_Data) {
		stbi_image_free(m_Data);
	}
	glDeleteTextures(1, &PictureDescriptor);
}
