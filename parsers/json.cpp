
#include <parsers/json.h>

void muzzley::fromstream(istream& _in, JSONObj& _out) {
	muzzley::JSONParser _p;
	_p.switchRoots(&_out);
	_p.switchStreams(_in);
	_p.parse();
}

void muzzley::fromstr(string& _in, JSONObj& _out) {
	istringstream _ss;
	_ss.str(_in);
	muzzley::JSONParser _p;
	_p.switchRoots(&_out);
	_p.switchStreams(_ss);
	_p.parse();

}

void muzzley::fromstr(string& _in, JSONArr& _out) {
	istringstream _ss;
	_ss.str(_in);
	muzzley::JSONParser _p;
	_p.switchRoots(NULL, &_out);
	_p.switchStreams(_ss);
	_p.parse();
}

void muzzley::fromstr(string& _in, JSONElement** _out, muzzley::JSONType* _type )  {
	istringstream _ss;
	_ss.str(_in);

	muzzley::JSONParser _p;
	JSONObj _o;
	JSONArr _a;
	_p.switchRoots(&_o, &_a);
	_p.switchStreams(_ss);
	_p.parse();

	if (_p.d_scanner.__root_type == muzzley::JSObject) {
		*_type = muzzley::JSObject;
		*_out = _o.get();
	}
	else {
		*_type = muzzley::JSArray;
		*_out = _a.get();
	}
}

void muzzley::fromfile(ifstream& _in, JSONObj& _out) {
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		_p.switchRoots(&_out);
		_p.switchStreams(_in);
		_p.parse();
	}
}

void muzzley::fromfile(ifstream& _in, JSONArr& _out) {
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		_p.switchRoots(NULL, &_out);
		_p.switchStreams(_in);
		_p.parse();
	}
}

void muzzley::fromfile(ifstream& _in, JSONElement** _out, muzzley::JSONType* _type )  {
	if (_in.is_open()) {
		muzzley::JSONParser _p;
		JSONObj _o;
		JSONArr _a;
		_p.switchRoots(&_o, &_a);
		_p.switchStreams(_in);
		_p.parse();

		if (_p.d_scanner.__root_type == muzzley::JSObject) {
			*_type = muzzley::JSObject;
			*_out = _o.get();
		}
		else {
			*_type = muzzley::JSArray;
			*_out = _a.get();
		}
	}
}

void muzzley::tostr(string& _out, JSONElement& _in)  {
	_in.stringify(_out, _in.flags());
}

void muzzley::tostr(string& _out, JSONObj& _in)  {
	_in->stringify(_out, _in->flags());
}

void muzzley::tostr(string& _out, JSONArr& _in)  {
	_in->stringify(_out, _in->flags());
}
