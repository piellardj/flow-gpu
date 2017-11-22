#include "Parser.hpp"


#include <regex>
#include <iostream>
#include <sstream>


namespace Parser
{

	bool parseWindowSize(std::string& str, glm::uvec2& size)
	{
		std::regex sizeRegex("\n*Window size=(0*[1-9][0-9]*)x(0*[1-9][0-9]*)\n+");
		std::smatch matches;

		if (!std::regex_search(str, matches, sizeRegex, std::regex_constants::match_continuous)) {
			std::cerr << "Couldn't parse window size. The pattern is\nWindow size=SIZEXxSIZEY" << std::endl;
			return false;
		}

		size.x = stringToUint(matches[1]);
		size.y = stringToUint(matches[2]);
		std::cout << "Parsed window size: " << size.x << "x" << size.y << std::endl;
		str = matches.suffix();
		return true;
	}

	bool parseScene(std::string& str, Scene::Type& scene)
	{
		std::regex sceneRegex("Mode=(EDIT|PLAY)\n+");
		std::smatch matches;

		if (!std::regex_search(str, matches, sceneRegex, std::regex_constants::match_continuous)) {
			std::cerr << "Couldn't parse mode. The pattern is\nMode=[EDIT or PLAY]" << std::endl;
			return false;
		}

		scene = (matches[1] == "EDIT") ? Scene::Type::EDIT : Scene::Type::PLAY;
		std::cout << "Parsed scene type " << matches[1] << std::endl;
		str = matches.suffix();
		return true;
	}

	bool parseEditScene(std::string& str, EditScene::Description& description)
	{
		std::regex reg("Save to=\"(.*)\"\n+Background=\"(.*)\"\n+");
		std::smatch matches;

		if (!std::regex_search(str, matches, reg, std::regex_constants::match_continuous)) {
			std::cerr << "Couldn't parse save destination or background filename. ";
			std::cerr << "The pattern is:\nSave to=\"filename\"\nBackground=\"filename\n" << std::endl;
			return false;
		}

		description.saveFilename = matches[1];
		description.backgroundFilename = matches[2];

		str = matches.suffix();

		std::regex loadRegex("Load from=\"(.*)\"\n*");
		if (std::regex_search(str, matches, loadRegex, std::regex_constants::match_continuous)) {
			description.flowmapFilename = matches[1];
		}
		else {
			std::regex resRegex("Flow map resolution=(0*[1-9][0-9]*)x(0*[1-9][0-9]*)\n*");
			if (!std::regex_search(str, matches, resRegex, std::regex_constants::match_continuous)) {
				std::cerr << "Couldn't parse the file to load or the wanted flow map resolution. ";
				std::cerr << "The pattern is\nLoad from=\"filename\" or\nFlow map resolution=SIZEXxSIZEY" <<  std::endl;
				return false;
			}
			description.defaultResolution.x = stringToUint(matches[1]);
			description.defaultResolution.y = stringToUint(matches[2]);
		}
		return true;
	}

	bool parsePlayScene(std::string& str, PlayScene::Description& layers)
	{
		std::regex layerRegex("Layer:\nBackground=\"(.*)\"\nFlow map=\"(.*)\"\nNb particles=(0*[1-9][0-9]*)\n*");
		std::smatch matches;

		while (std::regex_search(str, matches, layerRegex, std::regex_constants::match_continuous)) {
			unsigned int sqrtNbPart = (unsigned int)(std::sqrt(stringToUint(matches[3])));
			layers.emplace_back(matches[1], matches[2], sqrtNbPart);
			str = matches.suffix();
		}

		if (layers.empty()) {
			std::cerr << "Couldn't parse any layers. The pattern is:\nLayer:\nBackground=\"filename\"\nFlow map=\"filename\"" << std::endl;
		}
		return !layers.empty();
	}

	unsigned int stringToUint(std::string const& s)
	{
		unsigned int result;
		std::istringstream oss(s);
		oss >> result;
		return result;
	}
}