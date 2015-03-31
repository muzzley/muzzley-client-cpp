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

#include <muzzley/base/assert.h>

std::string muzzley::stacktrace() {
	void* _frames[MAX_FRAMES];
	string _text;
	size_t _fsize = backtrace(_frames, MAX_FRAMES);
	char ** _stack(backtrace_symbols(_frames, _fsize));
	for (size_t _i = 0 ; _i != _fsize ; ++_i ) {
		if (_i != 0) {
			_text.insert(_text.length(), "\n");
		}
		_text.insert(_text.length(), _stack[_i]);
	}
	free(_stack);
	return _text;
}

extern "C" int muzzley_lib() {
	return 1;
}