#include <iostream>
#include <memory>

#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "EditScene.hpp"
#include "PlayScene.hpp"
#include "IO.hpp"
#include "Parser.hpp"

static void initGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err)
		std::cerr << "Error while initializing GLEW: " << glewGetErrorString(err) << std::endl;
}

static void displayGLInfo()
{
	std::cout << "Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Using OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	glm::uvec2 windowSize(1280, 720);
//#define REGEX_WORK
#ifdef REGEX_WORK
	std::string setupFilename = (argc == 2) ? argv[1] : "setup.txt";
	std::string setupStr;
	if (!IO::loadFileToStr(setupFilename, setupStr)) {
		return EXIT_SUCCESS;
	}

	std::cout << "Loading setup file '" << setupFilename << "'" << std::endl;

	if (!Parser::parseWindowSize(setupStr, windowSize)) {
		return EXIT_SUCCESS;
	}
#endif

	/* Window creation, OpenGL initialization */
	sf::ContextSettings openGL3DContext(24, 0, 0, //depth, stencil, antialiasing
		3, 3, sf::ContextSettings::Core); //OpenGL 3.3 core
	sf::Window window;
	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Flow", sf::Style::Titlebar | sf::Style::Close, openGL3DContext);
	window.setVerticalSyncEnabled(true);
	initGLEW();

	/* Scene parsing and creation */
	std::unique_ptr<Scene> scene;
#ifdef REGEX_WORK
	Scene::Type sceneType;
	if (!Parser::parseScene(setupStr, sceneType)) {
		return EXIT_SUCCESS;
	}
	if (sceneType == Scene::Type::EDIT) {
		std::string backgroundFilename;
		EditScene::Description description = { "", "", "", glm::uvec2(128,128) };
		if (!Parser::parseEditScene(setupStr, description)) {
			return EXIT_SUCCESS;
		}
		scene.reset(new EditScene(description, window));
	}
	else {
		PlayScene::Description description;
		if (!Parser::parsePlayScene(setupStr, description)) {
			return EXIT_SUCCESS;
		}
		scene.reset(new PlayScene(description, window));
	}
#else
    PlayScene::Description description;
    description.emplace_back("rc/portrait/layer0_background.png", "rc/portrait/layer0_flowmap.txt", 200);
    description.emplace_back("rc/portrait/layer1_background.png", "rc/portrait/layer1_flowmap.txt", 128);
    description.emplace_back("rc/portrait/layer2_background.png", "rc/portrait/layer2_flowmap.txt", 128);
        description.emplace_back("rc/portrait/layer3_background.png", "rc/portrait/layer3_flowmap.txt", 200);
    scene.reset(new PlayScene(description, window));
#endif

	displayGLInfo();

	/* Main loop */
	bool running = true;
	unsigned int loops = 0u;
	sf::Clock overallFPS;
	while (running) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			}

			scene->handleEvent(event, window);
		}

		scene->update(overallFPS.getElapsedTime().asSeconds());

		window.setActive(true);
		glViewport(0, 0, window.getSize().x, window.getSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scene->display();
		window.display();

		++loops;
	}

	std::cout << "=== " << static_cast<float>(loops) / overallFPS.getElapsedTime().asSeconds() << " overall fps ===" << std::endl;

	scene.release(); //must be called before the window closes

	return EXIT_SUCCESS;
}
