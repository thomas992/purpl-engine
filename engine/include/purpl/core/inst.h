#pragma once

#include <memory>
#include <string>
#include <set>

#include "purpl/core/coredefs.h"
#include "purpl/core/log.h"

#ifdef PURPL_BUILD
// Forward declarations for graphics
namespace purpl::graphics
{

class window;

}
#endif

namespace purpl::core
{

#ifdef PURPL_BUILD
/// Internal engine context
class engine_instance {
    public:
	/// When the engine was initialized
	chrono::time_point<chrono::system_clock> start_time;

	// Loggers
	std::shared_ptr<log::logger> core_logger; /// Core logger
	std::shared_ptr<log::logger> gfx_logger; /// Graphics logger

	/// List of windows
	std::set<purpl::graphics::window *> windows;

	/// Constructor
	engine_instance();

	/// Destructor
	~engine_instance();
};

/// Global engine instance
extern std::unique_ptr<engine_instance> inst;
#endif // PURPL_BUILD

}
