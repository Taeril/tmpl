#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

// https://github.com/catchorg/Catch2

#include <tmpl/tmpl.hpp>

TEST_CASE("Clear") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ a }} {{ b }} {{ c }})~");

	auto root = tmpl.data();
	root->set("a", "1");
	root->set("b", "2");
	root->set("c", "3");

	auto str = tmpl.make();
	CHECK(str == "1 2 3");

	root->clear();
	root->set("a", "foo");
	root->set("b", "bar");
	root->set("c", "baz");

	auto str2 = tmpl.make();
	CHECK(str2 == "foo bar baz");
}

