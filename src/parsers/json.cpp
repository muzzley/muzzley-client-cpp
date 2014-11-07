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


#include <muzzley/parsers/json.h>

void muzzley::fromstream(istream& _in, JSONObj& _out) {
	muzzley::JSONParser _p;
	JSONPtr _root(new JSONElementT(_out));
	_p.switchRoots(_root);
	_p.switchStreams(_in);
	_p.parse();
}

void muzzley::fromstr(string& _in, JSONObj& _out) {
	istringstream _ss;
	_ss.str(_in);
	muzzley::JSONParser _p;
	JSONPtr _root(new JSONElementT(_out));
	_p.switchRoots(_root);
	_p.switchStreams(_ss);
	_p.parse();

}

void muzzley::fromstr(string& _in, JSONArr& _out) {
	istringstream _ss;
	_ss.str(_in);
	muzzley::JSONParser _p;
	JSONPtr _root(new JSONElementT(_out));
	_p.switchRoots(_root);
	_p.switchStreams(_ss);
	_p.parse();
}

void muzzley::fromfile(ifstream& _in, JSONObj& _out) {
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		JSONPtr _root(new JSONElementT(_out));
		_p.switchRoots(_root);
		_p.switchStreams(_in);
		_p.parse();
	}
}

void muzzley::fromfile(ifstream& _in, JSONArr& _out) {
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		JSONPtr _root(new JSONElementT(_out));
		_p.switchRoots(_root);
		_p.switchStreams(_in);
		_p.parse();
	}
}

muzzley::JSONPtr muzzley::fromstr(string& _in) {
	istringstream _ss;
	_ss.str(_in);
	muzzley::JSONParser _p;
	JSONPtr _root;
	_p.switchRoots(_root);
	_p.switchStreams(_ss);
	_p.parse();
	return _root;
}

muzzley::JSONPtr muzzley::fromfile(ifstream& _in) {
	JSONPtr _root;
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		_p.switchRoots(_root);
		_p.switchStreams(_in);
		_p.parse();
	}
	return _root;
}

void muzzley::tostr(string& _out, JSONElement& _in)  {
	_in->stringify(_out);
}

void muzzley::tostr(string& _out, JSONObj& _in)  {
	_in->stringify(_out);
}

void muzzley::tostr(string& _out, JSONArr& _in)  {
	_in->stringify(_out);
}
