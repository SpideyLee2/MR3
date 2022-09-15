#include "Texture2D.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

Texture2D::Texture2D(const std::string& dir, const bool& isHDR) 
	: m_Directory(dir), m_IsHDR(isHDR), m_BoundTexUnit(-1), m_IsBound(false) {

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	//glGenTextures(1, &m_ID);
	//bind(0);

	// TODO: Have user provide a sampler for this
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (isHDR) {
		float* texData = stbi_loadf(dir.c_str(), &m_Width, &m_Height, &m_NumChannels, 0);
		if (texData) {
			glTextureStorage2D(m_ID, 1, GL_RGB16F, m_Width, m_Height);
			glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_FLOAT, texData);
			glGenerateTextureMipmap(m_ID);
		}
		else {
			std::cout << "ERROR: Failed to load 2D *HDR* texture data from " << dir << std::endl;
		}
		stbi_image_free(texData);
	}
	else {
		stbi_uc* texData = stbi_load(dir.c_str(), &m_Width, &m_Height, &m_NumChannels, 0);
		if (texData) {
			switch (m_NumChannels) {
				case 1:
					glTextureStorage2D(m_ID, 1, GL_R8, m_Width, m_Height);
					glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RED, GL_UNSIGNED_BYTE, texData);
					break;
				case 2:
					glTextureStorage2D(m_ID, 1, GL_RG8, m_Width, m_Height);
					glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RG, GL_UNSIGNED_BYTE, texData);
					break;
				case 3:
					if (isHDR) {
						glTextureStorage2D(m_ID, 1, GL_RGB16F, m_Width, m_Height);
						glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_FLOAT, texData);
					}
					else {
						glTextureStorage2D(m_ID, 1, GL_RGB8, m_Width, m_Height);
						glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, texData);
					}
					break;
				case 4:
					glTextureStorage2D(m_ID, 1, GL_RGBA8, m_Width, m_Height);
					glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, texData);
					break;
				default:
					std::cout << "How does your texture have more than 4 channels?\n";
			}

			glGenerateTextureMipmap(m_ID);
		}
		else {
			std::cout << "ERROR: Failed to load 2D texture data from " << dir << std::endl;
		}

		stbi_image_free(texData);
	}
}

void Texture2D::bind(const unsigned int& texUnit) const {
	m_BoundTexUnit = texUnit;
	m_IsBound = true;
	glBindTextureUnit(texUnit, m_ID);
}

void Texture2D::unbind() const {
	if (!m_IsBound) {
		std::cout << "Texture " << m_ID << " is not bound to any texture unit. It cannot be unbound.\n";
		return;
	}
	glBindTextureUnit(m_BoundTexUnit, 0);
	m_BoundTexUnit = -1;
	m_IsBound = false;
}

GLuint Texture2D::getID() const {
	return m_ID;
}

std::string Texture2D::getTexDir() const {
	return m_Directory;
}

// Copy constructor
Texture2D::Texture2D(const Texture2D& tex2d) : 
	m_ID(tex2d.m_ID),
	m_BoundTexUnit(tex2d.m_BoundTexUnit),
	m_Directory(tex2d.m_Directory),
	m_IsHDR(tex2d.m_IsHDR),
	m_Width(tex2d.m_Width),
	m_Height(tex2d.m_Height),
	m_NumChannels(tex2d.m_NumChannels),
	m_IsBound(tex2d.m_IsBound) {
}

// Copy operator
Texture2D& Texture2D::operator=(const Texture2D& tex2d) {
	if (this != &tex2d) {
		m_ID = tex2d.m_ID,
		m_BoundTexUnit = tex2d.m_BoundTexUnit;
		m_Directory = tex2d.m_Directory;
		m_IsHDR = tex2d.m_IsHDR;
		m_Width = tex2d.m_Width;
		m_Height = tex2d.m_Height;
		m_NumChannels = tex2d.m_NumChannels;
		m_IsBound = tex2d.m_IsBound;
	}
	return *this;
}

// Move constructor
Texture2D::Texture2D(Texture2D&& tex2d) noexcept : 
	m_ID(std::move(tex2d.m_ID)),
	m_BoundTexUnit(std::move(tex2d.m_BoundTexUnit)),
	m_Directory(std::move(tex2d.m_Directory)),
	m_IsHDR(std::move(tex2d.m_IsHDR)),
	m_Width(std::move(tex2d.m_Width)),
	m_Height(std::move(tex2d.m_Height)),
	m_NumChannels(std::move(tex2d.m_NumChannels)),
	m_IsBound(std::move(tex2d.m_IsBound)) {
}

// Move operator
Texture2D& Texture2D::operator=(Texture2D&& tex2d) noexcept {
	if (this != &tex2d) {
		m_ID = std::move(tex2d.m_ID);
		m_BoundTexUnit = std::move(tex2d.m_BoundTexUnit);
		m_Directory = std::move(tex2d.m_Directory);
		m_IsHDR = std::move(tex2d.m_IsHDR);
		m_Width = std::move(tex2d.m_Width);
		m_Height = std::move(tex2d.m_Height);
		m_NumChannels = std::move(tex2d.m_NumChannels);
		m_IsBound = std::move(tex2d.m_IsBound);
	}
	return *this;
}

Texture2D::~Texture2D() {
	std::cout << "Deleting texture generated from " << m_Directory << std::endl;
	//glDeleteTextures(1, &m_ID);
}