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
	
	itc::Shader  shader3d;
	Texture mageTexture;
	unique_ptr<BMDModel> mageModel;
	Vertex3dBuffer mageVertexBuff;

	////////// Scene ///////////
	Sprite  itcSprite;
	Text    mccTitle;
	vec3    magePos;
	vec3    mageRot;
	vec3    cameraPos;
	vec3    cameraTarget;
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

		// load a 3d mesh
		mageTexture.loadFromFile("statue_mage.bmp");
		mageModel = BMDModel::loadFromFile("statue_mage.bmd");
		mageVertexBuff.create(mageModel->vertices(), mageModel->num_verts, 
							  mageModel->indices(), mageModel->num_indices);

		shader3d.loadShader("simple"); // loads simple.vert and simple.frag
	}

	void setupScene()
	{
		auto size = getSize();
		itcSprite.setTexture(itcTexture, true);

		createText(mccTitle, neoretro, "Mooncascade", 64);
		auto frame = mccTitle.getLocalBounds();
		mccTitle.setOrigin(frame.width / 2, frame.height / 2);
		mccTitle.setPosition(size.x / 2.0f, size.y * 0.66f);

		// setup 3d scene
		magePos = vec3::ZERO;
		mageRot = vec3::ZERO;
		cameraPos    = vec3{0, 12, 12};
		cameraTarget = vec3{0, 5, 0};
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
		// create a viewProjectionMatrix
		mat4 proj, look;
		proj.perspective(45.0f, (float)getSize().x, (float)getSize().y, 0.1f, 10000.0f);
		proj.multiply(look.lookat(cameraPos, cameraTarget, vec3{0.0f,1.0f,0.0f}));

		// create transformation matrix
		mat4 modelWorld;
		modelWorld.from_position(modelWorld, magePos);
		modelWorld.scale({1.0f,1.0f,1.0f});
		mat4 rot;
		mat4::from_rotation(rot, mageRot);
		modelWorld.multiply(rot);

		// create a modelViewProjection matrix
		mat4 modelViewProjection = proj;
		modelViewProjection.multiply(modelWorld);

		shader3d.bind();
		shader3d.bind(u_DiffuseTex, 0, mageTexture);
		shader3d.bind(u_Transform, modelViewProjection);
		mageVertexBuff.draw();
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
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);  // clear background to soft black
	
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
		float deltaTime = clock.restart().asSeconds();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		game.draw3d(deltaTime);
		game.pushGLStates();
		game.drawGui(deltaTime);
        game.display();
		game.popGLStates();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////