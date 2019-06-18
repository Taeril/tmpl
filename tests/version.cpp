#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <string>
#include "tmpl.hpp"

TEST_CASE("Version") {
	std::string version = tmpl::version();
	std::string ver
		= std::to_string(tmpl::version_major()) + "."
		+ std::to_string(tmpl::version_minor()) + "."
		+ std::to_string(tmpl::version_patch());

	CHECK(version == ver);
}

