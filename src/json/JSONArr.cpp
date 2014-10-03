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


#include <json/JSONObj.h>

#include <iostream>
#include <exceptions/CastException.h>
#include <exceptions/NoAttributeNameException.h>

muzzley::JSONArrRef::JSONArrRef() {
}

muzzley::JSONArrRef::~JSONArrRef() {
	for (JSONArrRef::iterator i = this->begin(); i != this->end(); i++) {
		delete *i;
	}
}

muzzley::JSONType muzzley::JSONArrRef::type() {
	return muzzley::JSArray;
}

void muzzley::JSONArrRef::unset(string _in) {
}

void muzzley::JSONArrRef::unset(long long _in) {
	if (_in < (long long) this->size()) {
		this->erase(this->begin() + _in);
	}
}

void muzzley::JSONArrRef::put(int _in) {
	JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(long _in) {
	JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(long long _in) {
	JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

#ifdef __LP64__
void muzzley::JSONArrRef::put(unsigned int _in) {
	JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}
#endif

void muzzley::JSONArrRef::put(size_t _in) {
	JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(double _in) {
	JSONDbl* _sp = new JSONDbl(new JSONDblRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(bool _in) {
	JSONBool* _sp = new JSONBool(new JSONBoolRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(string _in) {
	JSONStr* _sp = new JSONStr(new JSONStrRef(_in));
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(JSONObj& _in) {
	JSONObj* _sp = new JSONObj(_in.get());
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(JSONArr& _in) {
	JSONArr* _sp = new JSONArr(_in.get());
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

void muzzley::JSONArrRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONArrRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONArrRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONArrRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONArrRef::put(JSONNil& _in) {
	JSONNil* _sp = new JSONNil(new JSONNilRef());
	this->push_back((smart_ptr<JSONElement>*) _sp);
}

bool muzzley::JSONArrRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this == &_in;
}

muzzley::JSONElement& muzzley::JSONArrRef::get(size_t _idx) {
	if(_idx < this->size()) {
		return *this->at(_idx)->get();
	}
	return JSON_NIL;
}

muzzley::JSONElement& muzzley::JSONArrRef::get(const char* _idx) {
	return *this;
}

int muzzley::JSONArrRef::getInt() {
	return this->size();
}

long muzzley::JSONArrRef::getLong() {
	return this->size();
}

long muzzley::JSONArrRef::getLongLong() {
	return this->size();
}

unsigned int muzzley::JSONArrRef::getUnsignedInt() {
	return this->size();
}

double muzzley::JSONArrRef::getDouble() {
	return this->size();
}

bool muzzley::JSONArrRef::getBool() {
	return true;
}

string muzzley::JSONArrRef::getString() {
	string _ret;
	this->stringify(_ret, this->__flags);
	return _ret;
}

muzzley::JSONObjRef& muzzley::JSONArrRef::getJSONObj() {
	throw CastException("can not convert from JSONArr to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONArrRef::getJSONArr() {
	return *this;
}

void muzzley::JSONArrRef::stringify(ostream& _out, short _flags, string _tabs) {
	string _ret;
	this->stringify(_ret, _flags, _tabs);
	_out << _ret << flush;
}

void muzzley::JSONArrRef::stringify(string& _out, short _flags, string _tabs) {
	_out.insert(_out.length(), "[");
	if (_flags & muzzley::pretty) {
		_tabs.insert(_tabs.length(), "\t");
	}
	bool first = true;
	for (JSONArrRef::iterator i = this->begin(); i != this->end(); i++) {
		if (!first) {
			_out.insert(_out.length(), ",");
		}
		first = false;
		if (_flags & muzzley::pretty) {
			_out.insert(_out.length(), "\n");
			_out.insert(_out.length(), _tabs);
		}
		(*i)->get()->stringify(_out, _flags, _tabs);
	}
	if (_flags & muzzley::pretty) {
		_out.insert(_out.length(), "\n");
		_tabs .erase(_tabs.length() - 1, 1);
	}
	_out.insert(_out.length(),  _tabs);
	_out.insert(_out.length(),  "]");
}

muzzley::JSONElement& muzzley::JSONArrRef::operator[](int _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONArrRef::operator[](size_t _idx) {
	return this->get(_idx);
}
