#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <string>
#include <cctype>
#include <algorithm>

#include <tmpl/tmpl.hpp>

TEST_CASE("default") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ str }})~");

	auto root = tmpl.data();
	root->set("str", R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");

	auto str = tmpl.make();
	CHECK(str == R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");
}

TEST_CASE("raw") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ str|raw }})~");

	auto root = tmpl.data();
	root->set("str", R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");

	auto str = tmpl.make();
	CHECK(str == R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");
}

TEST_CASE("html") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ str|html }})~");

	auto root = tmpl.data();
	root->set("str", R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");

	auto str = tmpl.make();
	CHECK(str == R"~(&lt;p&gt;Foo's &quot;test&quot; = 42 &amp;&amp; +-_.~&lt;/p&gt;)~");
}

TEST_CASE("url") {
	tmpl::Template tmpl;
	tmpl.parse(R"~({{ str|url }})~");

	auto root = tmpl.data();
	root->set("str", R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");

	auto str = tmpl.make();
	CHECK(str == R"~(%3Cp%3EFoo%27s%20%22test%22%20%3D%2042%20%26%26%20%2B-_.~%3C%2Fp%3E)~");
}

TEST_CASE("custom") {
	tmpl::Template tmpl;
	tmpl.add_filter("rot13", [](std::string const& str) -> std::string {
		std::string ret(str.size(), '\0');
		std::transform(str.begin(), str.end(), ret.begin(),
			[](unsigned char c) -> unsigned char {
				int n = islower(c) ? 97 : 65;
				return isalpha(c) ? (unsigned char)(n + (c - n + 13) % 26) : c;
			});
		return ret;
	});
	tmpl.parse(R"~({{ str|rot13 }})~");

	auto root = tmpl.data();
	root->set("str", R"~(<p>Foo's "test" = 42 && +-_.~</p>)~");

	auto str = tmpl.make();
	CHECK(str == R"~(<c>Sbb'f "grfg" = 42 && +-_.~</c>)~");
}

