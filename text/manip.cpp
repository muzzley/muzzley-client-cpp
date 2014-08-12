/*
Copyright (c) 2014, Muzzley

Permission to use, copy, modify, and/or distribute this software for 
any purpose with or without fee is hereby granted, provided that the 
above copyright notice and this permission notice appear in all 
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
PERFORMANCE OF THIS SOFTWARE.
*/


#include <stddef.h>
#include <text/manip.h>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iterator>
#include <string>

using namespace std;
using namespace __gnu_cxx;

void muzzley::ltrim(std::string &_in_out) {
        _in_out.erase(_in_out.begin(), std::find_if(_in_out.begin(), _in_out.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void muzzley::rtrim(std::string &_in_out) {
        _in_out.erase(std::find_if(_in_out.rbegin(), _in_out.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), _in_out.end());
}

void muzzley::trim(std::string &_in_out) {
        muzzley::ltrim(_in_out);
        muzzley::rtrim(_in_out);
}

void muzzley::replace(string& str, string find, string replace) {
	if (str.length() == 0) {
		return;
	}

	size_t start = 0;

	while ((start = str.find(find, start)) != string::npos) {
		str.replace(start, find.size(), replace);
		start += replace.length();
	}
}

