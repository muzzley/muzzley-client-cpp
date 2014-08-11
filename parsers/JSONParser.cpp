
#include <parsers/JSONParser.h>

muzzley::JSONParser::JSONParser(std::istream &_in, std::ostream &_out, muzzley::JSONObj* _rootobj, muzzley::JSONArr* _rootarr) {
	this->d_scanner.switchStreams(_in, _out);
	this->d_scanner.switchRoots(_rootobj, _rootarr);
}

muzzley::JSONParser::~JSONParser() {
}

void muzzley::JSONParser::switchRoots(JSONObj* _rootobj, JSONArr* _rootarr) {
	this->d_scanner.switchRoots(_rootobj, _rootarr);
}

void muzzley::JSONParser::switchStreams(std::istream &_in, std::ostream &_out) {
	this->d_scanner.switchStreams(_in, _out);
}

