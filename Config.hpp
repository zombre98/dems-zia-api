/*
** EPITECH PROJECT, 2021
** zia
** File description:
** Config.hpp
*/

#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace dems::config {

	struct ConfigValue;
	using ConfigObject = std::map<std::string, ConfigValue>;
	using ConfigArray = std::vector<ConfigValue>;

	/**
	* Represents a configuration value.
	*/
	struct ConfigValue {
		std::variant<std::monostate, ConfigObject, ConfigArray, std::string, long long, double, bool> v;
	};

	/**
	* Configiguration (format influenced by JSON).
	* Configiguration must contain:
	*  - "modules": must contain modules with at least these settings:
	*	- path: path of the library
	*/
	using Config = ConfigObject;
}
