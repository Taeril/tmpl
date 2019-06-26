#include "utils.hpp"

#include <fmt/core.h>

std::string url_encode(std::string const& value) {
	std::string ret;
	std::string::size_type m = 0;
	std::string::size_type n = 0;

	const char* letters = "0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz" "-_.~";

	while((n = value.find_first_not_of(letters, m)) != std::string::npos) {
		ret.append(value.substr(m, n-m));
		ret.append(fmt::format("%{:02X}", static_cast<unsigned char>(value[n])));

		m = n + 1;
	}
	ret.append(value.substr(m));

	return ret;
}

std::string html_escape(std::string const& value) {
	std::string ret;
	std::string::size_type m = 0;
	std::string::size_type n = 0;

	//const char* escape = "&<>\"'";
	const char* escape = "&<>\"";

	while((n = value.find_first_of(escape, m)) != std::string::npos) {
		ret.append(value.substr(m, n-m));
		switch(value[n]) {
			case '&': ret.append("&amp;"); break;
			case '<': ret.append("&lt;"); break;
			case '>': ret.append("&gt;"); break;
			case '"': ret.append("&quot;"); break;
			default:
				ret.append(fmt::format("&#{:d};", static_cast<unsigned char>(value[n])));
		}

		m = n + 1;
	}
	ret.append(value.substr(m));

	return ret;
}

