//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <string>
#include "Stage.hpp"

namespace dems {

/**
 * @class AModulesManager
 * Abstract class to implement for loading the differents module
 */
class AModulesManager {
public:
	/**
	 * The destructor
	 */
	virtual ~AModulesManager() = default;
	/**
	 * Load all the modules present in a directory
	 * @param directoryPath the path to the directory containing the modules
	 */
	virtual void loadModules(const std::string &directoryPath) = 0;
	/**
	 * Load one module
	 * @param filePath the path to the module
	 */
	virtual void loadOneModule(const std::string &filePath) = 0;

	/**
	* Unload a Module
	* @param moduleName The module to unload
	*/
	virtual void unloadModule(const std::string &moduleName) = 0;

	/**
	 * Get the Stage Manager
	 * @return Stage Manager
	 */
	StageManager &getStageManager() { return stageManager_; }

private:
	StageManager stageManager_;
};

}
