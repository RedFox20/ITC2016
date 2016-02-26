#include <SFML/Graphics.hpp>
#include "util.hpp"
#include "Model.hpp"
using namespace itc;

////////////////////////////////////////////////////////////////////////////////

struct ITC2016 : public RenderWindow
{
	//////// Resources /////////
	Texture itcTexture;
	Font    neoretro;
	Font    neoretroShadow;
	Font    dejavusans;

	////////// Scene ///////////
	Sprite  itcSprite;
	Text    mccTitle;
	vector<Vertex> path;


	ITC2016(ContextSettings& settings) 
		: RenderWindow(VideoMode(1280, 720), "ITC2016", Style::Close, settings)
	{
	}

	void loadResources()
	{
		loadTexture(itcTexture, "itc2016.png");
		neoretro.loadFromFile("neoretro.ttf");
		neoretroShadow.loadFromFile("neoretro-shadow.ttf");
		dejavusans.loadFromFile("dejavusans.ttf");
	}

	void setupScene()
	{
		auto size = getSize();
		itcSprite.setTexture(itcTexture, true);

		createText(mccTitle, neoretro, "Mooncascade", 64);
		auto frame = mccTitle.getLocalBounds();
		mccTitle.setOrigin(frame.width / 2, frame.height / 2);
		mccTitle.setPosition(size.x / 2.0f, size.y * 0.66f);
	}

	void drawText(Text& text, const Font& primary, const Color& mainColor, 
							  const Font& shadow, const Color& shadowColor)
	{
		text.setFont(neoretroShadow); // draw shadow
		text.setColor(shadowColor);
		draw(text);
		text.setFont(neoretro); // then color
		text.setColor(mainColor);
		draw(text);
	}

	void drawGui(float deltaTime)
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			Vector2i pos = Mouse::getPosition(*this);
			path.push_back(Vertex(Vector2f((float)pos.x, (float)pos.y), Color::White));
		}
		draw(itcSprite);

		// update MCC text
		mccTitle.rotate(10.0f * deltaTime); // 10 deg/s
		drawText(mccTitle, neoretro, Color(255,255,0), neoretroShadow, Color(64,64,64,168));

		draw(path.data(), path.size(), PrimitiveType::LinesStrip);
	}

	void draw3d(float deltaTime)
	{
		// sorry; SFML was a bad dog
	}
};


////////////////////////////////////////////////////////////////////////////////

int main()
{
	// configure OpenGL 4.3
	ContextSettings settings;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 3;

	ITC2016 game { settings };
	game.setFramerateLimit(60);
	
	////////////// Init GLEW /////////////
	glewExperimental = true; // enable loading experimental OpenGL features
	GLenum status = glewInit();
	if (status != GLEW_OK) { // init GL extension wrangler
		fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(status));
		return EXIT_FAILURE;
	}

	//// Load game resources
	Clock clock;
	game.loadResources();
	game.setupScene();

	// enter game loop
    while (game.isOpen())
    {
        Event event;
        while (game.pollEvent(event))
        {
			if (event.type == Event::Closed || (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::Escape))) {
                game.close();
			}
        }
		game.clear(Color(64,64,64));
		float deltaTime = clock.restart().asSeconds();
		game.drawGui(deltaTime);
		game.display();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////