
#pragma once

#include <string>

using namespace std;
using namespace __gnu_cxx;

namespace muzzley {

	void ltrim(std::string &_in_out);
	void rtrim(std::string &_in_out);
	void trim(std::string &_in_out);
	void replace(string& str, string find, string replace);
}
