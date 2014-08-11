
#include <exceptions/ClosedException.h>

muzzley::ClosedException::ClosedException(string _in) : __what(_in){
}

muzzley::ClosedException::~ClosedException() throw() {
}

const char* muzzley::ClosedException::what() {
	return this->__what.data();
}
