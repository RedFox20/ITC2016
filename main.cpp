#include <SFML/Graphics.hpp>
#include "util.hpp"
using namespace itc;

////////////////////////////////////////////////////////////////////////////////

struct ITC2016 : public RenderWindow
{
	Texture itcTexture;
	Sprite  itcSprite;

	ITC2016() : RenderWindow(VideoMode(1280, 720), "ITC2016")
	{
		string s;
		loadTexture(itcTexture, "itc2016.png");
		itcSprite.setTexture(itcTexture, true);
	}

	void enterFrame(double deltaTime)
	{
		draw(itcSprite);
	}
};

////////////////////////////////////////////////////////////////////////////////

int main()
{
	Clock clock;
	ITC2016 game;
    while (game.isOpen())
    {
        Event event;
        while (game.pollEvent(event))
        {
            if (event.type == Event::Closed)
                game.close();
        }
        game.clear(Color(60, 60, 60));
		game.enterFrame(clock.restart().asMicroseconds() / 1000000.0);
        game.display();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////