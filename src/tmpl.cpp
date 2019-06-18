#include "tmpl.hpp"

#include <stack>
#include <optional>

#include <fmt/core.h>

#include "utils.hpp"
#include "version.hpp"

void hr(char c='-', size_t n=80) {
	for(size_t i=0; i<n; ++i) fmt::print("{}", c);
	fmt::print("\n");
}

namespace tmpl {

// version
std::string version() { return VERSION; }
unsigned version_major() { return VERSION_MAJOR; }
unsigned version_minor() { return VERSION_MINOR; }
unsigned version_patch() { return VERSION_PATCH; }

// Data

Data::Value& Data::add() {
	values_.push_back(Value{});
	return values_.back();
}

Data::Value& Data::add(std::initializer_list<std::pair<std::string, std::string>> values) {
	values_.push_back(Value{});
	auto& v = values_.back();

	for(auto& p : values) {
		v.set(p.first, p.second);
	}

	return v;
}

size_t Data::size() {
	return values_.size();
}

std::vector<Data::Value>& Data::values() {
	return values_;
}

void Data::print(std::string pre) {
	size_t n = 0;
	for(auto& v : values_) {
		auto h = fmt::format("{}-- « {} » ", pre, n);
		fmt::print("{}", h);
		hr('-', 2 + 80 - h.size()); // 2 for « and »
		v.print(pre);
		++n;
	}
}


// Data::Value

Data::Value& Data::Value::set(std::string const& key, std::string const& value) {
	value_[key] = value;
	return *this;
}

Data* Data::Value::block(std::string const& key) {
	value_[key] = std::make_unique<Data>();
	return std::get<std::unique_ptr<Data>>(value_[key]).get();
}


bool Data::Value::has_key(std::string const& key) {
	return value_.find(key) != value_.end();
}

bool Data::Value::is_string(std::string const& key) {
	auto p = value_.find(key);
	return p != value_.end() && std::holds_alternative<std::string>(p->second);
}

bool Data::Value::is_block(std::string const& key) {
	//auto p = value_.find(key);
	//return p != value_.end() && !std::holds_alternative<std::string>(p->second);
	return !is_string(key);
}


Data::ValueType* Data::Value::get(std::string const& key) {
	auto p = value_.find(key);
	if(p == value_.end()) {
		return nullptr;
	}

	return &(p->second);
}


void Data::Value::print(std::string pre) {
	for(auto& p : value_) {
		if(std::holds_alternative<std::string>(p.second)) {
			fmt::print("{}{} → {}\n", pre, p.first, std::get<std::string>(p.second));
		} else {
			fmt::print("{}{}[]:\n", pre, p.first);
			std::get<std::unique_ptr<Data>>(p.second).get()->print(pre + "    ");
		}
	}
}

// Template

enum class State {
	Nothing,
	Start,
	PreName,
	Name,
	PostName,
	End,
};

Template::Template() {
	default_filters();
}

Template::Template(std::string const& tmpl) {
	default_filters();

	parse(tmpl);
}


Data::Value* Template::data() {
	return &data_;
}


void Template::add_filter(std::string const& name, Template::FilterCallback filter) {
	filters_[name] = filter;
}

void Template::default_filter(Template::FilterCallback filter) {
	default_filter_ = filter;
}

void Template::default_filter(std::string const& name) {
	auto p = filters_.find(name);
	if(p != filters_.end()) {
		default_filter_ = p->second;
	}
}

void Template::default_filters() {
	filters_.clear();
	add_filter("html", html_escape);
	add_filter("url", url_encode);
	add_filter("raw", [](std::string const& v){ return v; });
}

void Template::parse(std::string const& tmpl) {
	////fmt::print("{}\n", tmpl);
	State state = State::Nothing;
	char ch = ' '; // '}' or '%'
	size_t pos = 0;

	size_t value_start = 0;
	size_t value_end = 0;
	size_t name_start = 0;
	size_t name_end = 0;

	std::stack<Block*> blocks;
	blocks.push(&root_);

	for(auto const& c : tmpl) {
		//fmt::print("{:12s} ({}) '{}' | {} {}-{}\n",
		//	state_str(state), ch, c, pos, value_start, value_end);

		if(state == State::Nothing) {
			if(c == '{') {
				value_end = pos;
				state = State::Start;
			}
		} else if(state == State::Start) {
			if(c == '{' || c == '%') {
				ch = c == '{' ? '}' : '%';
				state = State::PreName;
			} else {
				state = State::Nothing;
			}
		} else if(state == State::PreName) {
			if(
				(c >= 'a' && c <= 'z') || 
				(c >= 'A' && c <= 'Z') || 
				c == '_'
			) {
				name_start = pos;
				state = State::Name;
			} else if(c != ' ' && c != '\t') {
				state = State::Nothing;
			}
		} else if(state == State::Name) {
			if(
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') ||
				c == '_' || c == '-' ||
				c == '.' || c == '|'
			) {
				// do nothing
			} else if(c == ' ' || c == '\t') {
				name_end = pos;
				state = State::PostName;
			} else {
				state = State::Nothing;
			} 
		} else if(state == State::PostName) {
			if(c == ch) {
				state = State::End;
			} else if(c != ' ' && c != '\t') {
				state = State::Nothing;
			}
		} else if(state == State::End) {
			if(c == '}') {
				if(value_start < value_end) {
					auto value = tmpl.substr(value_start, value_end - value_start);
					//fmt::print(" - STR«{}»\n", value);
					blocks.top()->elements.push_back(String{value});
				}
				if(ch == '%') {
					auto name = tmpl.substr(name_start, name_end - name_start);
					if(name == "end") {
						//fmt::print(" - END«{}»\n", name);
						blocks.pop();
					} else {
						//fmt::print(" - BLOCK«{}»\n", name);
						blocks.top()->elements.push_back(Block{name, {}});
						blocks.push(&std::get<Block>(blocks.top()->elements.back()));

					}
				} else {
					auto name = tmpl.substr(name_start, name_end - name_start);
					FilterCallback filter;

					auto n = name.find('|');
					if(n != std::string::npos) {
						auto p = filters_.find(name.substr(n+1));
						if(p != filters_.end()) {
							filter = p->second;
						}
						name = name.substr(0, n);
					}

					//fmt::print(" - VAR«{}»\n", name);
					blocks.top()->elements.push_back(Variable{name, filter});
				}
				value_start = pos + 1;
			}
			state = State::Nothing;
		}

		++pos;
	}
	value_end = pos;
	if(value_start < value_end) {
		auto value = tmpl.substr(value_start, value_end - value_start);
		//fmt::print(" = STR«{}»\n", value);
		blocks.top()->elements.push_back(String{value});
	}
}


std::string Template::make() {
	std::vector<Data::Value*> vars;
	vars.push_back(&data_);

	return make_block(root_, vars);
}

Data::ValueType* find_data(std::vector<Data::Value*>& vars, std::string const& name) {
	for(auto it = rbegin(vars); it != rend(vars); ++it) {
		auto d = (*it)->get(name);
		if(d) {
			return d;
		}
	}

	return nullptr;
}

std::string Template::make_block(Block const& block, std::vector<Data::Value*>& vars) {
	std::string ret;

	for(auto const& e : block.elements) {
		if(std::holds_alternative<String>(e)) {
			////fmt::print("String: «{}»\n", std::get<String>(e).value);
			ret.append(std::get<String>(e).value);
		} else if(std::holds_alternative<Variable>(e)) {
			////fmt::print("Variable: «{}»\n", std::get<Variable>(e).name);
			auto name = std::get<Variable>(e).name;
			auto filter = std::get<Variable>(e).filter;

			auto v = find_data(vars, name);
			if(v) {
				if(std::holds_alternative<std::string>(*v)) {
					auto s = std::get<std::string>(*v);
					ret.append(filter ? filter(s) :
						default_filter_ ? default_filter_(s) : s
					);
				}
			}
		} else if(std::holds_alternative<Block>(e)) {
			auto name = std::get<Block>(e).name;
			////fmt::print("Block: «{}»\n", name);

			auto b = find_data(vars, name);
			if(b) {
				if(std::holds_alternative<std::string>(*b)) {
					Data::Value block_vars;
					vars.push_back(&block_vars);
					block_vars.set("__COUNT__", "0");
					block_vars.set("__IDX__", "0");
					block_vars.set("__NUM__", "1");
					ret.append(make_block(std::get<Block>(e), vars));
					vars.pop_back();
				} else {
					auto data = std::get_if<std::unique_ptr<Data>>(b);

					Data::Value block_vars;
					vars.push_back(&block_vars);
					block_vars.set("__COUNT__", fmt::format("{}", data->get()->size()));
					size_t idx = 0;

					for(auto& v : data->get()->values()) {
						block_vars.set("__IDX__", fmt::format("{}", idx));
						block_vars.set("__NUM__", fmt::format("{}", idx+1));
						vars.push_back(&v);
						ret.append(make_block(std::get<Block>(e), vars));
						vars.pop_back();
						++idx;
					}
					vars.pop_back();
				}
			}
		}
	}

	return ret;
}

} // namespace tmpl

