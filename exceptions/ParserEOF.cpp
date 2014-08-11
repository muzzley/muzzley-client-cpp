
#include <exceptions/ParserEOF.h>

muzzley::ParserEOF::ParserEOF(string _in) : __what(_in){
}

muzzley::ParserEOF::~ParserEOF() throw() {
}

const char* muzzley::ParserEOF::what() {
	return this->__what.data();
}
