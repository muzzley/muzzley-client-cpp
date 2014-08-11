
#include <exceptions/SyntaxErrorException.h>

muzzley::SyntaxErrorException::SyntaxErrorException(string _in) : __what(_in){
}

muzzley::SyntaxErrorException::~SyntaxErrorException() throw() {
}

const char* muzzley::SyntaxErrorException::what() {
	return this->__what.data();
}
