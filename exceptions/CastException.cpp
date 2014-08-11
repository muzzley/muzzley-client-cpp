
#include <exceptions/CastException.h>

muzzley::CastException::CastException(string _in) : __what(_in){
}

muzzley::CastException::~CastException() throw() {
}

const char* muzzley::CastException::what() {
	return this->__what.data();
}
