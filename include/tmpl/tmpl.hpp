#ifndef HEADER_TMPL_H
#define HEADER_TMPL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include <initializer_list>
#include <utility>
#include <functional>
#include <deque>

namespace tmpl {

std::string version();
unsigned version_major();
unsigned version_minor();
unsigned version_patch();

class Data {
	public:
		typedef std::variant<
			std::string,
			std::unique_ptr<Data>
		> ValueType;

		class Value;

		Value& add();
		Value& add(std::initializer_list<std::pair<std::string, std::string>> values);

		size_t size();
		std::vector<Value>& values();

		void print(std::string pre="");
	private:
		std::vector<Value> values_;
};

class Data::Value {
	public:

		Value& set(std::string const& key, std::string const& value);
		Data* block(std::string const& key);

		bool has_key(std::string const& key);
		bool is_string(std::string const& key);
		bool is_block(std::string const& key);

		ValueType* get(std::string const& key);

		void print(std::string pre="");

		void clear();
	private:
		std::unordered_map<
			std::string,
			ValueType
		> value_;
};

class Template {
	public:
		typedef std::function<std::string(std::string const&)> FilterCallback;
		Template();
		Template(std::string const& tmpl);

		Data::Value* data();

		void add_filter(std::string const& name, FilterCallback filter);
		void default_filter(FilterCallback filter);
		void default_filter(std::string const& name);

		void parse(std::string const& tmpl);

		std::string make();

	private:
		struct String {
			std::string value;
		};

		struct Variable {
			std::string name;
			FilterCallback filter;
		};

		struct Block {
			std::string name;
			std::deque<std::variant<String, Variable, Block>> elements;
		};

		Data::Value data_;
		Block root_;
		std::unordered_map<
			std::string,
			FilterCallback
		> filters_;
		FilterCallback default_filter_;

		void default_filters();

		std::string make_block(Block const& block, std::vector<Data::Value*>& vars);
};

} // namespace tmpl

#endif /* HEADER_TMPL_H */

