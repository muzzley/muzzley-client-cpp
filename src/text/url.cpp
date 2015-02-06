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

/*
   Unsafe:

   Characters can be unsafe for a number of reasons.  The space
   character is unsafe because significant spaces may disappear and
   insignificant spaces may be introduced when URLs are transcribed or
   typeset or subjected to the treatment of word-processing programs.
   The characters "<" and ">" are unsafe because they are used as the
   delimiters around URLs in free text; the quote mark (""") is used to
   delimit URLs in some systems.  The character "#" is unsafe and should
   always be encoded because it is used in World Wide Web and in other
   systems to delimit a URL from a fragment/anchor identifier that might
   follow it.  The character "%" is unsafe because it is used for
   encodings of other characters.  Other characters are unsafe because
   gateways and other transport agents are known to sometimes modify
   such characters. These characters are "{", "}", "|", "\", "^", "~",
   "[", "]", and "`".
*/
void muzzley::url_encode(string& _out) {
	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	const unsigned char * pSrc = (const unsigned char *) _out.c_str();
	const int SRC_LEN = _out.length();
	unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
	unsigned char * pEnd = pStart;
	const unsigned char * const SRC_END = pSrc + SRC_LEN;

	for (; pSrc < SRC_END; ++pSrc) {
		if ((*pSrc > 127) || (*pSrc == '"') || (*pSrc == '{') || (*pSrc == '}') || (*pSrc == '|') || (*pSrc == '\\') || (*pSrc == '^') || (*pSrc == '~') || (*pSrc == '[') || (*pSrc == ']') || (*pSrc == '`') || (*pSrc == '%') || (*pSrc == ' ') || (*pSrc == '&') || (*pSrc == '+') || (*pSrc == '?') || (*pSrc == '#') || (*pSrc == '=') || (*pSrc == '/') || (*pSrc == ':')) {
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[*pSrc >> 4];
			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
		}
		else {
			*pEnd++ = *pSrc;
		}
	}

	std::string sResult((char *) pStart, (char *) pEnd);
	delete[] pStart;
	_out.assign(sResult);
}

void muzzley::url_decode(string& _out) {
	muzzley::replace(_out, "+", "%20");

	const unsigned char * pSrc = (const unsigned char *) _out.c_str();
	const int SRC_LEN = _out.length();
	const unsigned char * const SRC_END = pSrc + SRC_LEN;
	const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

	char * const pStart = new char[SRC_LEN];
	char * pEnd = pStart;

	while (pSrc < SRC_LAST_DEC) {
		if (*pSrc == '%') {
			stringstream ss;
			ss << *(pSrc + 1) << *(pSrc + 2);
			int c;
			ss >> hex >> c;
			*pEnd++ = (char) c;
			pSrc += 3;
			continue;
		}

		*pEnd++ = *pSrc++;
	}

	// the last 2- chars
	while (pSrc < SRC_END) {
		*pEnd++ = *pSrc++;
	}
	std::string sResult(pStart, pEnd);
	delete[] pStart;
	_out.assign(sResult);
}
