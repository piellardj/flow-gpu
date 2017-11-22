#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED


#include <memory>
#include <regex>

#include "glm.hpp"
#include "PlayScene.hpp"
#include "EditScene.hpp"


namespace Parser
{
	bool parseWindowSize(std::string& str, glm::uvec2& size);
	bool parseScene(std::string& str, Scene::Type& scene);
	bool parseEditScene(std::string& str, EditScene::Description& description);
	bool parsePlayScene(std::string& str, PlayScene::Description& description);

	unsigned int stringToUint(std::string const& s);
}

#endif // PARSER_HPP_INCLUDED
