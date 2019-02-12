//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <string>
#include "Stage.hpp"

/**
 * @brief dems namespace
 */
namespace dems {

/**
 * @class AModulesManager
 * Abstract class to implement for loading the different module
 */
class AModulesManager {
public:
	/**
	 * The destructor
	 */
	virtual ~AModulesManager() = default;
	/**
	 * Loads all the modules present in a directory in numerical, then alphabetical order
	 * (e.g. 01_modssl.so then modphp.so then modzlib.so)
	 * @param directoryPath the path to the directory containing the modules
	 */
	virtual void loadModules(const std::string &directoryPath) = 0;
	/**
	 * Loads one module
	 * @param filePath the path to the module
	 */
	virtual void loadOneModule(const std::string &filePath) = 0;

	/**
	* Unloads a Module
	* @param moduleName The module to unload
	*/
	virtual void unloadModule(const std::string &moduleName) = 0;

	/**
	 * Gets the Stage Manager
	 * @return Stage Manager
	 */
	StageManager &getStageManager() { return stageManager_; }

private:
	StageManager stageManager_;
};

}
