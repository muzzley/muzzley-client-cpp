
#include <exceptions/AssertionException.h>

#include <sstream>
#include <text/convert.h>

muzzley::AssertionException::AssertionException(string _in, int _http_code, int _code, string _desc, int _line, string _file) : __what(_in), __http_code(_http_code), __code(_code), __description(_desc), __line(_line), __file(_file){
	this->__description.insert(0, "'");
	this->__description.insert(this->__description.length(), "' failed on file ");
	this->__description.insert(this->__description.length(), this->__file);
	this->__description.insert(this->__description.length(),  ", line ");
	muzzley::tostr(this->__description, this->__line);
}

muzzley::AssertionException::~AssertionException() throw() {
}

const char* muzzley::AssertionException::what() {
	return this->__what.data();
}

const char* muzzley::AssertionException::description() {
	return this->__description.data();
}

int muzzley::AssertionException::code() {
	return this->__code;
}

int muzzley::AssertionException::status() {
	return this->__http_code;
}
