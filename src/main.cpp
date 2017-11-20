#include <iostream>
#include <memory>

#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "EditMode.hpp"
#include "PlayMode.hpp"


static void initGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err)
		std::cerr << "Error while initializing GLEW: " << glewGetErrorString(err) << std::endl;

	std::cout << "Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
}

int main()
{
	/* Window creation, OpenGL initialization */
	sf::ContextSettings openGL3DContext(24, 0, 0, //depth, stencil, antialiasing
		3, 3, //openGL 3.3 requested
		sf::ContextSettings::Core);
	sf::Window window;
	window.create(sf::VideoMode(800, 800), "Flow", sf::Style::Titlebar | sf::Style::Close, openGL3DContext);
	window.setVerticalSyncEnabled(true);
	initGLEW();
	
	std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Using OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;

	std::unique_ptr<Mode> mode;
	const std::string flowmapStr = "rc/layer1_flowmap.txt";
	const std::string backStr = "rc/layer1_background.png";

//#define EDIT
#ifdef EDIT
	{
		std::unique_ptr<EditMode> tmp;
		tmp.reset(new EditMode(flowmapStr, window, flowmapStr));
		tmp->setBackground(backStr);
		mode = std::move(tmp);
	}
#else
	std::vector<PlayMode::LayerFilename> filenames;
	filenames.emplace_back("rc/layer0_background.png", "rc/layer0_flowmap.txt");
	filenames.emplace_back("rc/layer1_background.png", "rc/layer1_flowmap.txt");
	filenames.emplace_back("rc/layer2_background.png", "rc/layer2_flowmap.txt");
	mode.reset(new PlayMode(filenames, window));
#endif

	bool running = true;
	unsigned int loops = 0u;
	sf::Clock instantFPS, overallFPS;
	while (running) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			}

			mode->handleEvent(event, window);
		}

		mode->update(overallFPS.getElapsedTime().asSeconds());
		
		window.setActive(true);
		glViewport(0, 0, window.getSize().x, window.getSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mode->display();

		window.display();

		++loops;
	}

	std::cout << "=== " << static_cast<float>(loops) / overallFPS.getElapsedTime().asSeconds() << " overall fps ===" << std::endl;

	mode.release();
	window.close();
	{
		char c;
		std::cin >> c;
	}

	return EXIT_SUCCESS;
}