#include "util.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace itc
{
	bool loadTexture(Texture& outTexture, const string& filename)
	{
		if (outTexture.loadFromFile(filename)) {
			outTexture.setSmooth(true);
			return true;
		}
		return false;
	}

	Text& createText(Text& outText, const Font& font, const string& str, int size)
	{
		outText.setFont(font);
		outText.setString(str);
		outText.setCharacterSize(size);
		return outText;
	}

}

////////////////////////////////////////////////////////////////////////////////