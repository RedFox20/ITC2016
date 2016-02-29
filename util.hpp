#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

namespace itc
{
	using namespace std; // using included with our namespace
	using namespace sf;


	/** @return true if the texture was loaded */
	bool loadTexture(Texture& outTexture, const string& filename);

	/** @brief Simplifies Text creation */
	Text& createText(Text& outText, const Font& font, const string& str, int size);
}

////////////////////////////////////////////////////////////////////////////////