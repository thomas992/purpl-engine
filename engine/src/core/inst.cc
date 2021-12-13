// Instance instantiation functions
//
// Copyright 2021 MobSlicer152
// This file is part of Purpl Engine
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "purpl/core/inst.h"

namespace purpl::core
{

std::unique_ptr<engine_instance> inst;

const std::string &PURPL_DEFAULT_LOGGER_PATTERN = "[PID %P, TID %t] [%D %a %T] [%n@%l] %v";

engine_instance::engine_instance()
{
	// Set up the loggers
	this->start_time = chrono::system_clock::now();
	this->core_logger = purpl::log::basic_logger_mt("core", "purpl.log");
	this->core_logger->set_pattern(PURPL_DEFAULT_LOGGER_PATTERN);
	this->gfx_logger = purpl::log::basic_logger_mt("graphics", "purpl.log");
	this->gfx_logger->set_pattern(PURPL_DEFAULT_LOGGER_PATTERN);
}

engine_instance::~engine_instance() {}

}
