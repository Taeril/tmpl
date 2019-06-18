#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

// https://github.com/catchorg/Catch2

#include "tmpl.hpp"

TEST_CASE("Empty") {
	tmpl::Template tmpl;
	auto str = tmpl.make();
	CHECK(str == "");
}

TEST_CASE("Variables") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ a }} {{ b }} {{ c }})~");

	auto root = tmpl.data();
	root->set("a", "1");
	root->set("b", "2");
	root->set("c", "3");

	SECTION("has key") {
		CHECK(root->has_key("a"));
		CHECK(root->has_key("b"));
		CHECK(root->has_key("c"));
		CHECK_FALSE(root->has_key("d"));
	}

	SECTION("is string") {
		CHECK(root->is_string("a"));
		CHECK(root->is_string("b"));
		CHECK(root->is_string("c"));
	}

	SECTION("is block") {
		CHECK_FALSE(root->is_block("a"));
		CHECK_FALSE(root->is_block("b"));
		CHECK_FALSE(root->is_block("c"));
	}

	auto str = tmpl.make();
	CHECK(str == "1 2 3");
}

TEST_CASE("Blocks") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({% block %}#{% end %})~");

	auto root = tmpl.data();
	auto block = root->block("block");
	for(size_t i=0; i<4; ++i) {
		block->add();
	}

	CHECK_FALSE(root->is_string("asdf"));
	CHECK(root->is_block("asdf"));

	auto str = tmpl.make();
	CHECK(str == "####");
}

TEST_CASE("Block Variables") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({% block %}[{{ __IDX__ }}: {{ test }} {{ __NUM__ }}/{{ __COUNT__ }}] {% end %})~");

	auto root = tmpl.data();
	auto block = root->block("block");
	for(size_t i=0; i<3; ++i) {
		block->add().set("test", "TEST");
	}

	auto str = tmpl.make();
	CHECK(str == R"~([0: TEST 1/3] [1: TEST 2/3] [2: TEST 3/3] )~");
}

TEST_CASE("Nested Blocks") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({% outer %}[{{ __IDX__ }}: {% inner %}{{ test }}{% end %}] {% end %})~");

	char values[] = "ABC";

	auto root = tmpl.data();
	auto outer = root->block("outer");
	for(size_t i=0; i<3; ++i) {
		auto& outer_data = outer->add();
		auto inner = outer_data.block("inner"); 
		inner->add().set("test", std::string(values+i));
	}

	auto str = tmpl.make();
	CHECK(str == R"~([0: ABC] [1: BC] [2: C] )~");
}

