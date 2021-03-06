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


#include <muzzley/text/convert.h>

#include <unistd.h>
#include <iomanip>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#define BOM8A 0xEF
#define BOM8B 0xBB
#define BOM8C 0xBF

char* muzzley::wstring_to_utf8(wstring ws) {
	string dest;
	dest.clear();
	for (size_t i = 0; i < ws.size(); i++) {
		wchar_t w = ws[i];
		if (w <= 0x7f)
		dest.push_back((char) w);
		else if (w <= 0x7ff) {
			dest.push_back(0xc0 | ((w >> 6) & 0x1f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (w <= 0xffff) {
			dest.push_back(0xe0 | ((w >> 12) & 0x0f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (w <= 0x10ffff) {
			dest.push_back(0xf0 | ((w >> 18) & 0x07));
			dest.push_back(0x80 | ((w >> 12) & 0x3f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else
		dest.push_back('?');
	}

	char* c = new char[dest.length() + 1];
	memset(c, 0, dest.length() + 1);
	memcpy(c, dest.c_str(), dest.length());
	return c;
}

wchar_t* muzzley::utf8_to_wstring(string s) {
	long b = 0, c = 0;
	const char* string = s.data();

	if ((uchar) string[0] == BOM8A && (uchar) string[1] == BOM8B && (uchar) string[2] == BOM8C) {
		string += 3;
	}

	for (const char *a = string; *a; a++) {
		if (((uchar) *a) < 128 || (*a & 192) == 192) {
			c++;
		}
	}
	wchar_t *res = new wchar_t[c + 1];
	res[c] = 0;
	for (uchar *a = (uchar*) string; *a; a++) {
		if (!(*a & 128)) {
			res[b] = *a;
		}
		else if ((*a & 192) == 128) {
			continue;
		}
		else if ((*a & 224) == 192) {
			res[b] = ((*a & 31) << 6) | (a[1] & 63);
		}
		else if ((*a & 240) == 224) {
			res[b] = ((*a & 15) << 12) | ((a[1] & 63) << 6) | (a[2] & 63);
		}
		else if ((*a & 248) == 240) {
			res[b] = '?';
		}
		b++;
	}
	return res;
}

int muzzley::utf8_length(string s) {
	int size = 0;
	for (size_t i = 0; i != s.length(); i++) {
		if (((wchar_t) s[i]) < 0x80) {
			size++;
		}
		else if (((wchar_t) s[i]) < 0x800) {
			size += 2;
		}
		else {
			size += 3;
		}
	}
	return size;
}

void muzzley::utf8_encode(wstring s, string& _out, bool quote) {
	ostringstream oss;

	for (size_t i = 0; i != s.length(); i++) {
		if (((int) s[i]) > 127) {
			oss << "\\u" <<  setfill('0') << setw(4) << hex << ((int) s.at(i));
		}
		else if (quote && (s[i] == '"')) {
			oss << "\\" << ((char) s.at(i));
		}
		else if (s[i] == '\n') {
			oss << "\\n";
		}
		else if (s[i] == '\r') {
			oss << "\\r";
		}
		else if (s[i] == '\f') {
			oss << "\\f";
		}
		else if (s[i] == '\t') {
			oss << "\\t";
		}
		else if (s[i] == '/') {
			oss << "\\/";
		}
		else if (quote && s[i] == '\\') {
			oss << "\\\\";
		}
		else if (((int) s[i]) <= 31) {
			oss << "";
		}
		else {
			oss << ((char) s.at(i));
		}
	}
	oss << flush;
	_out.assign(oss.str().data(), oss.str().length());
}

void muzzley::utf8_encode(string& _out, bool quote) {
	long b = 0, c = 0;
	const char* string = _out.data();

	if ((uchar) string[0] == BOM8A && (uchar) string[1] == BOM8B && (uchar) string[2] == BOM8C) {
		string += 3;
	}

	for (const char *a = string; *a; a++) {
		if (((uchar) *a) < 128 || (*a & 192) == 192) {
			c++;
		}
	}
	wchar_t *res = new wchar_t[c + 1];
	res[c] = 0;
	for (uchar *a = (uchar*) string; *a; a++) {
		if (!(*a & 128)) {
			res[b] = *a;
		}
		else if ((*a & 192) == 128) {
			continue;
		}
		else if ((*a & 224) == 192) {
			res[b] = ((*a & 31) << 6) | (a[1] & 63);
		}
		else if ((*a & 240) == 224) {
			res[b] = ((*a & 15) << 12) | ((a[1] & 63) << 6) | (a[2] & 63);
		}
		else if ((*a & 248) == 240) {
			res[b] = '?';
		}
		b++;
	}

	wstring ws;
	ws.assign(res, c + 1);
	muzzley::utf8_encode(ws, _out, quote);
	delete[] res;

}

void muzzley::utf8_decode(string& _out) {
	wostringstream oss;
	for (size_t i = 0; i != _out.length(); i++) {
		if (_out[i] == '\\' && _out[i + 1] == 'u') {
			stringstream ss;
			ss << _out[i + 2] << _out[i + 3] << _out[i + 4] << _out[i + 5];
			int c;
			ss >> hex >> c;
			oss << ((wchar_t) c);
			i += 5;
		}
		else {
			oss << ((wchar_t) _out[i]);
		}
	}
	oss << flush;

	char* c = muzzley::wstring_to_utf8(oss.str());
	string os(c);
	_out.assign(c);

	delete[] c;
}
