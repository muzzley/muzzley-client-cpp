
#include <exceptions/NoAttributeNameException.h>

muzzley::NoAttributeNameException::NoAttributeNameException(string _in) : __what(_in){
}

muzzley::NoAttributeNameException::~NoAttributeNameException() throw() {
}

const char* muzzley::NoAttributeNameException::what() {
	return this->__what.data();
}
