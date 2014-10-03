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

muzzley::JSONObjRef::JSONObjRef() {
	this->__name = NULL;
}

muzzley::JSONObjRef::~JSONObjRef() {
	if (this->__name != NULL) {
		delete this->__name;
		this->__name = NULL;
	}
}

muzzley::JSONType muzzley::JSONObjRef::type() {
	return muzzley::JSObject;
}

void muzzley::JSONObjRef::unset(string _in) {
	JSONObjRef::iterator _i;
	if ((_i = this->find(_in)) != this->end()) {
		return this->erase(_i);
	}
}

void muzzley::JSONObjRef::unset(long long _in) {
}

void muzzley::JSONObjRef::put(int _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(long _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(long long _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

#ifdef __LP64__
void muzzley::JSONObjRef::put(unsigned int _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}
#endif

void muzzley::JSONObjRef::put(size_t _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONInt* _sp = new JSONInt(new JSONIntRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(double _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		size_t dot = this->__name->find(".");
		if (dot != string::npos) {
			this->__name->erase(dot, 1);
			this->__name->insert(dot, "_");
		}
		this->__name->insert(0, "_");
	}
	else {
		JSONDbl* _sp = new JSONDbl(new JSONDblRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(bool _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		muzzley::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		JSONBool* _sp = new JSONBool(new JSONBoolRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(string _in) {
	if (this->__name == NULL) {
		this->__name = new string(_in);
	}
	else {
		JSONStr* _sp = new JSONStr(new JSONStrRef(_in));
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(JSONObj& _in) {
	if (this->__name == NULL) {
		throw muzzley::NoAttributeNameException("can't create attribute name from JS Object");
	}
	else {
		JSONObj* _sp = new JSONObj(_in.get());
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(JSONArr& _in) {
	if (this->__name == NULL) {
		throw muzzley::NoAttributeNameException("can't create attribute name from JS Array");
	}
	else {
		JSONArr* _sp = new JSONArr(_in.get());
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

void muzzley::JSONObjRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONObjRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONObjRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONObjRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONObjRef::put(JSONNil& _in) {
	if (this->__name == NULL) {
		this->__name = new string("null");
	}
	else {
		JSONNil* _sp = new JSONNil(new JSONNilRef());
		this->insert(string(this->__name->data()), (smart_ptr<JSONElement>*) _sp);
		delete this->__name;
		this->__name = NULL;
	}
}

bool muzzley::JSONObjRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this == &_in;
}

muzzley::JSONElement& muzzley::JSONObjRef::get(size_t _idx) {
	if(_idx < this->size()) {
		return *this->at(_idx)->get();
	}
	return JSON_NIL;
}

muzzley::JSONElement& muzzley::JSONObjRef::get(const char* _idx) {
	JSONObjRef::iterator i;
	if ((i = this->find(_idx)) != this->end()) {
		return *((*i)->second->get());
	}
	return JSON_NIL;
}

int muzzley::JSONObjRef::getInt() {
	return this->size();
}

long muzzley::JSONObjRef::getLong() {
	return this->size();
}

long muzzley::JSONObjRef::getLongLong() {
	return this->size();
}

unsigned int muzzley::JSONObjRef::getUnsignedInt() {
	return this->size();
}

double muzzley::JSONObjRef::getDouble() {
	return this->size();
}

bool muzzley::JSONObjRef::getBool() {
	return true;
}

string muzzley::JSONObjRef::getString() {
	string _ret;
	this->stringify(_ret, this->__flags);
	return _ret;
}

muzzley::JSONObjRef& muzzley::JSONObjRef::getJSONObj() {
	return *this;
}

muzzley::JSONArrRef& muzzley::JSONObjRef::getJSONArr() {
	throw CastException("can not convert from JSONObj to JSONArr");
}

void muzzley::JSONObjRef::stringify(ostream& _out, short _flags, string _tabs) {
	string _ret;
	this->stringify(_ret, _flags, _tabs);
	_out << _ret << flush;
}

void muzzley::JSONObjRef::stringify(string& _out, short _flags, string _tabs) {
	_out.insert(_out.length(),  "{");
	if (_flags & muzzley::pretty) {
		_tabs.insert(_tabs.length(), "\t");
	}
	bool first = true;
	for (JSONObjRef::iterator i = this->begin(); i != this->end(); i++) {
		if (!first) {
			_out.insert(_out.length(),  ",");
		}
		first = false;
		if (_flags & muzzley::pretty) {
			_out.insert(_out.length(),  "\n");
		}
		_out.insert(_out.length(),  _tabs);
		_out.insert(_out.length(),  "\"");
		_out.insert(_out.length(),  (*i)->first);
		_out.insert(_out.length(),  (_flags & muzzley::pretty ? "\" : " : "\":"));
		(*i)->second->get()->stringify(_out, _flags, _tabs);
	}
	if (_flags & muzzley::pretty) {
		_out.insert(_out.length(),  "\n");
		_tabs .erase(_tabs.length() - 1, 1);
	}
	_out.insert(_out.length(),  _tabs);
	_out.insert(_out.length(),  "}");
}

muzzley::JSONElement& muzzley::JSONObjRef::operator[](int _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONObjRef::operator[](size_t _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONObjRef::operator[](const char* _idx) {
	return this->get(_idx);
}









