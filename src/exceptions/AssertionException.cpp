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


#include <muzzley/exceptions/AssertionException.h>

#include <sstream>
#include <stdlib.h> 
#include <muzzley/text/convert.h>

muzzley::AssertionException::AssertionException(string _in, int _http_code, int _code, string _desc, int _line, string _file, char** _backtrace, size_t _backtrace_size) : __what( _in ), __http_code(_http_code), __code(_code), __description(_desc), __line(_line), __file(_file){
	this->__description.insert(0, "'");
	this->__description.insert(this->__description.length(), "' failed on file ");
	this->__description.insert(this->__description.length(), this->__file);
	this->__description.insert(this->__description.length(),  ", line ");
	muzzley::tostr(this->__description, this->__line);

	for (size_t _i = 0; _i != _backtrace_size; _i++) {
		this->__backtrace.insert(this->__backtrace.length(), string(_backtrace[_i]));
		this->__backtrace.insert(this->__backtrace.length(), "\n");
	}
	free(_backtrace);
}

muzzley::AssertionException::~AssertionException() throw() {
}

const char* muzzley::AssertionException::what() const throw() {
	return this->__what.data();
}

const char* muzzley::AssertionException::description() {
	return this->__description.data();
}

const char* muzzley::AssertionException::backtrace() {
	return this->__backtrace.data();
}

int muzzley::AssertionException::code() {
	return this->__code;
}

int muzzley::AssertionException::status() {
	return this->__http_code;
}
