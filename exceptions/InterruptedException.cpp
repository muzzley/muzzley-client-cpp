
#include <exceptions/InterruptedException.h>

muzzley::InterruptedException::InterruptedException(string _in) : __what(_in){
}

muzzley::InterruptedException::~InterruptedException() throw() {
}

const char* muzzley::InterruptedException::what() {
	return this->__what.data();
}
