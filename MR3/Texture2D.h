#pragma once

#include <iostream>
#include <unordered_map>

//enum class TEXTURE_TYPE {
//	DIFFUSE,
//	SPECULAR,
//	NORMAL
//};

class Texture2D {
public:
	Texture2D() = default;
	Texture2D(const std::string& dir, const bool& isHDR = false);
	~Texture2D();
	Texture2D(Texture2D&& tex2d) noexcept; // move constructor
	Texture2D(const Texture2D& tex2d); // copy operator

	Texture2D& operator=(Texture2D&& tex2d) noexcept; // move operator
	Texture2D& operator=(const Texture2D& tex2d); // copy constructor

	void bind(const unsigned int& texUnit) const;
	void unbind() const;

	unsigned getID() const;
	std::string getTexDir() const;
private:
	void create(const std::string& dir, const bool& isHDR);

	unsigned int m_ID;
	mutable bool m_IsBound;
	std::string m_Directory;
	bool m_IsHDR;
	int m_Width, m_Height, m_NumChannels;
	mutable unsigned int m_BoundTexUnit;
};