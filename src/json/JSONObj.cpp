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

#include <muzzley/json/JSONObj.h>

#include <ostream>
#include <muzzley/parsers/JSONParser.h>

namespace muzzley {
	JSONPtr undefined;
	JSONPtr nilptr = undefined;
}

muzzley::JSONElementT::JSONElementT() : __parent( nullptr ) {
	this->type(muzzley::JSNil);
	this->__target.__nil = nullptr;
}

muzzley::JSONElementT::JSONElementT(JSONElementT& _element) : __parent( nullptr ) {
	this->type( _element.type());
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			if (_element.obj().get() != nullptr) {
				this->__target.__object = _element.obj();
			}
			break;
		}
		case muzzley::JSArray : {
			if (_element.arr().get() != nullptr) {
				this->__target.__array = _element.arr();
			}
			break;
		}
		case muzzley::JSString : {
			this->__target.__string = make_shared<string>(_element.str());
			break;
		}
		case muzzley::JSInteger : {
			this->__target.__integer = _element.intr();
			break;
		}
		case muzzley::JSDouble : {
			this->__target.__double = _element.dbl();
			break;
		}
		case muzzley::JSBoolean : {
			this->__target.__boolean = _element.bln();
			break;
		}
		case muzzley::JSNil : {
			this->__target.__nil = nullptr;
			break;
		}
		case muzzley::JSDate : {
			this->__target.__date = _element.date();
			break;
		}
	}
}

muzzley::JSONElementT::JSONElementT(JSONPtr& _value) {
	this->type(_value->type());
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			if (_value->obj().get() != nullptr) {
				this->__target.__object = _value->obj();
			}
			break;
		}
		case muzzley::JSArray : {
			if (_value->arr().get() != nullptr) {
				this->__target.__array = _value->arr();
			}
			break;
		}
		case muzzley::JSString : {
			this->__target.__string = make_shared<string>(_value->str());
			break;
		}
		case muzzley::JSInteger : {
			this->__target.__integer = _value->intr();
			break;
		}
		case muzzley::JSDouble : {
			this->__target.__double = _value->dbl();
			break;
		}
		case muzzley::JSBoolean : {
			this->__target.__boolean = _value->bln();
			break;
		}
		case muzzley::JSNil : {
			this->__target.__nil = nullptr;
			break;
		}
		case muzzley::JSDate : {
			this->__target.__date = _value->date();
			break;
		}
	}
}

muzzley::JSONElementT::JSONElementT(JSONObj& _value) : __parent( nullptr ) {
	this->type( muzzley::JSObject);
	if (_value.get() != nullptr) {
		this->__target.__object = _value;
	}
}

muzzley::JSONElementT::JSONElementT(JSONArr& _value) : __parent( nullptr ) {
	this->type( muzzley::JSArray);
	if (_value.get() != nullptr) {
		this->__target.__array = _value;
	}
}

muzzley::JSONElementT::JSONElementT(string _value) : __parent( nullptr ) {
	this->type( muzzley::JSString);
	this->__target.__string = make_shared<string>(_value);
}

muzzley::JSONElementT::JSONElementT(const char* _value) : __parent( nullptr ) {
	this->type( muzzley::JSString);
	this->__target.__string = make_shared<string>(string(_value));
}

muzzley::JSONElementT::JSONElementT(long long _value) : __parent( nullptr ) {
	this->type( muzzley::JSInteger);
	this->__target.__integer = _value;
}

muzzley::JSONElementT::JSONElementT(double _value) : __parent( nullptr ) {
	this->type( muzzley::JSDouble);
	this->__target.__double = _value;
}

muzzley::JSONElementT::JSONElementT(bool _value) : __parent( nullptr ) {
	this->type( muzzley::JSBoolean);
	this->__target.__boolean = _value;
}

muzzley::JSONElementT::JSONElementT(time_t _value) : __parent( nullptr ) {
	this->type( muzzley::JSDate);
	this->__target.__date = _value;
}

muzzley::JSONElementT::JSONElementT(int _value) : __parent( nullptr ) {
	this->type( muzzley::JSInteger);
	this->__target.__integer = _value;
}

muzzley::JSONElementT::JSONElementT(size_t _value) : __parent( nullptr ) {
	this->type( muzzley::JSInteger);
	this->__target.__integer = _value;
}

#ifdef __LP64__
muzzley::JSONElementT::JSONElementT(unsigned int _value) : __parent( nullptr ) {
	this->type( muzzley::JSInteger);
	this->__target.__integer = _value;
}
#endif

muzzley::JSONElementT::~JSONElementT() {
}

muzzley::JSONType muzzley::JSONElementT::type() {
	return (muzzley::JSONType)  this->__target.__type;
}

void muzzley::JSONElementT::type(JSONType _in) {
	assertz(_in >= 0, "the type must be a valid value", 0, 0);
	
	if (_in == this->__target.__type) {
		return;
	}

	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object.~JSONObj();
			//delete & this->__target.__object;
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array.~JSONArr();
			//delete & this->__target.__array;
			break;
		}
		case muzzley::JSString : {
			this->__target.__string.~JSONStr();
			//delete & this->__target.__string;
			break;
		}
		default : {
			break;
		}
	}
	switch(_in) {
		case muzzley::JSObject : {
			new(& this->__target.__object) JSONObj();
			break;
		}
		case muzzley::JSArray : {
			new(& this->__target.__array) JSONArr();
			break;
		}
		case muzzley::JSString : {
			new(& this->__target.__string) JSONStr();
			break;
		}
		default : {
			break;
		}
		
	}

	this->__target.__type = _in;
}

muzzley::JSONUnion& muzzley::JSONElementT::value() {
	return this->__target;
}

bool muzzley::JSONElementT::ok() {
	return this->__target.__type != muzzley::JSNil;
}

bool muzzley::JSONElementT::empty() {
	return this->__target.__type == muzzley::JSNil;
}

bool muzzley::JSONElementT::nil() {
	return this->__target.__type == muzzley::JSNil;
}

void muzzley::JSONElementT::assign(JSONElementT& _rhs) {
	this->type( _rhs.type());
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			if (_rhs.obj().get() != nullptr) {
				this->__target.__object = _rhs.obj();
			}
			break;
		}
		case muzzley::JSArray : {
			if (_rhs.arr().get() != nullptr) {
				this->__target.__array = _rhs.arr();
			}
			break;
		}
		case muzzley::JSString : {
			this->__target.__string = make_shared<string>(_rhs.str());
			break;
		}
		case muzzley::JSInteger : {
			this->__target.__integer = _rhs.intr();
			break;
		}
		case muzzley::JSDouble : {
			this->__target.__double = _rhs.dbl();
			break;
		}
		case muzzley::JSBoolean : {
			this->__target.__boolean = _rhs.bln();
			break;
		}
		case muzzley::JSNil : {
			this->__target.__nil = nullptr;
			break;
		}
		case muzzley::JSDate : {
			this->__target.__date = _rhs.date();
			break;
		}
	}
}

muzzley::JSONElementT * muzzley::JSONElementT::parent() {
	return this->__parent;
}

void muzzley::JSONElementT::parent(JSONElementT* _parent) {
	this->__parent = _parent;
}

muzzley::JSONObj& muzzley::JSONElementT::obj() {
	assertz(this->__target.__type == muzzley::JSObject, "this element is not of type JSObject", 0, 0);
	return this->__target.__object;
}

muzzley::JSONArr& muzzley::JSONElementT::arr() {
	assertz(this->__target.__type == muzzley::JSArray, "this element is not of type JSArray", 0, 0);
	return this->__target.__array;
}

string muzzley::JSONElementT::str() {
	assertz(this->__target.__type == muzzley::JSString, "this element is not of type JSString", 0, 0);
	return *(this->__target.__string.get());
}

long long muzzley::JSONElementT::intr() {
	assertz(this->__target.__type == muzzley::JSInteger, "this element is not of type JSInteger", 0, 0);
	return this->__target.__integer;
}

double muzzley::JSONElementT::dbl() {
	assertz(this->__target.__type == muzzley::JSDouble, "this element is not of type JSDouble", 0, 0);
	return this->__target.__double;
}

bool muzzley::JSONElementT::bln() {
	assertz(this->__target.__type == muzzley::JSBoolean, "this element is not of type JSBoolean", 0, 0);
	return this->__target.__boolean;
}

time_t muzzley::JSONElementT::date() {
	assertz(this->__target.__type == muzzley::JSDate, "this element is not of type JSDate", 0, 0);
	return this->__target.__date;
}

double muzzley::JSONElementT::number() {
	assertz(this->__target.__type == muzzley::JSInteger || this->__target.__type == muzzley::JSDouble || this->__target.__type == muzzley::JSBoolean, "this element is not of type JSInteger, JSDouble or JSBoolean", 0, 0);
	switch(this->__target.__type) {
		case muzzley::JSInteger : {
			return (double) this->__target.__integer;
		}
		case muzzley::JSDouble : {
			return this->__target.__double;
		}
		case muzzley::JSBoolean : {
			return (double) this->__target.__boolean;
		}
	}
	return 0;
}

muzzley::JSONElementT& muzzley::JSONElementT::operator<<(const char* _in) {
	(*this) << string(_in);
	return * this;
}

muzzley::JSONElementT& muzzley::JSONElementT::operator<<(string _in) {
	assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->push(_in);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->push(new muzzley::JSONElementT(string(_in)));
			break;
		}
		default : {
			this->__target.__string.get()->assign(_in);
			this->type( muzzley::JSString);
			assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
			break;
		}
	}
	return * this;
}

muzzley::JSONElementT& muzzley::JSONElementT::operator<<(JSONElementT* _in) {
	assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->push(_in);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->push(_in);
			break;
		}
		default : {
			assertz(this->__target.__type == muzzley::JSObject || this->__target.__type == muzzley::JSArray, "the type must be a JSObject or JSArray in order to push JSONElementT*", 0, 0);
			break;
		}
	}
	return * this;
}

bool muzzley::JSONElementT::operator==(muzzley::JSONElementT& _in) {
	assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
	if (this->__target.__type !=  _in.type()) {
		return false;
	}
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			return *(this->__target.__object) == *(_in.obj());
		}
		case muzzley::JSArray : {
			return *(this->__target.__array) == *(_in.arr());
		}
		case muzzley::JSString : {
			return *(this->__target.__string.get()) == _in.str();
		}
		case muzzley::JSInteger : {
			return this->__target.__integer == _in.intr();
		}
		case muzzley::JSDouble : {
			return this->__target.__double == _in.dbl();
		}
		case muzzley::JSBoolean : {
			return this->__target.__boolean == _in.bln();
		}
		case muzzley::JSNil : {
			return true;
		}
		case muzzley::JSDate : {
			return this->__target.__date == _in.date();
		}
	}
	return false;
}

bool muzzley::JSONElementT::operator==(muzzley::JSONPtr& _rhs) {
	return * this == * _rhs;
}

bool muzzley::JSONElementT::operator!=(JSONElementT& _in) {
	assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
	if (this->__target.__type != _in.type()) {
		return true;
	}
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			return this->__target.__object != _in.obj();
		}
		case muzzley::JSArray : {
			return this->__target.__array != _in.arr();
		}
		case muzzley::JSString : {
			return *(this->__target.__string.get()) != _in.str();
		}
		case muzzley::JSInteger : {
			return this->__target.__integer != _in.intr();
		}
		case muzzley::JSDouble : {
			return this->__target.__double != _in.dbl();
		}
		case muzzley::JSBoolean : {
			return this->__target.__boolean != _in.bln();
		}
		case muzzley::JSNil : {
			return true;
		}
		case muzzley::JSDate : {
			return this->__target.__date != _in.date();
		}
	}
	return false;
}

bool muzzley::JSONElementT::operator!=(muzzley::JSONPtr& _rhs) {
	return * this != * _rhs;
}

void muzzley::JSONElementT::stringify(ostream& _out) {
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->stringify(_out);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->stringify(_out);
			break;
		}
		case muzzley::JSString : {
			string _str(this->str());
			muzzley::replace(_str, "\"", "\\\"");
			muzzley::replace(_str, "\n", "\\\n");
			muzzley::replace(_str, "\r", "\\\b");
			muzzley::replace(_str, "\t", "\\\t");
			_out << "\"" << _str << "\"" << flush;
			break;
		}
		case muzzley::JSInteger : {
			_out << this->__target.__integer << flush;
			break;
		}
		case muzzley::JSDouble : {
			_out << this->__target.__double << flush;
			break;
		}
		case muzzley::JSBoolean : {
			_out << (this->__target.__boolean ? "true" : "false") << flush;
			break;
		}
		case muzzley::JSNil : {
			_out <<  "undefined" << flush;
			break;
		}
		case muzzley::JSDate : {
			string _date;
			muzzley::tostr(_date, this->__target.__date, "%Y-%m-%dT%H:%M:%S.000Z");
			_out << "\"" << _date << "\"" << flush;
			break;
		}
	}
}

void muzzley::JSONElementT::stringify(string& _out) {
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->stringify(_out);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->stringify(_out);
			break;
		}
		case muzzley::JSString : {
			string _str(this->str());
			muzzley::replace(_str, "\"", "\\\"");
			muzzley::replace(_str, "\n", "\\\n");
			muzzley::replace(_str, "\r", "\\\b");
			muzzley::replace(_str, "\t", "\\\t");
			_out.insert(_out.length(), "\"");
			_out.insert(_out.length(), _str);
			_out.insert(_out.length(), "\"");
			break;
		}
		case muzzley::JSInteger : {
			muzzley::tostr(_out, this->__target.__integer);
			break;
		}
		case muzzley::JSDouble : {
			muzzley::tostr(_out, this->__target.__double);
			break;
		}
		case muzzley::JSBoolean : {
			muzzley::tostr(_out, this->__target.__boolean);
			break;
		}
		case muzzley::JSNil : {
			_out.insert(_out.length(), "undefined");
			break;
		}
		case muzzley::JSDate : {
			_out.insert(_out.length(), "\"");
			muzzley::tostr(_out, this->__target.__date, "%Y-%m-%dT%H-%M-%S.000Z");
			_out.insert(_out.length(), "\"");
			break;
		}
	}
}

void muzzley::JSONElementT::prettify(ostream& _out, uint _n_tabs) {
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->prettify(_out, _n_tabs);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->prettify(_out, _n_tabs);
			break;
		}
		case muzzley::JSString : {
			string _str(this->str());
			muzzley::replace(_str, "\"", "\\\"");
			muzzley::replace(_str, "\n", "\\\n");
			muzzley::replace(_str, "\r", "\\\b");
			muzzley::replace(_str, "\t", "\\\t");
			_out << "\"" << _str << "\"" << flush;
			break;
		}
		case muzzley::JSInteger : {
			_out << this->__target.__integer << flush;
			break;
		}
		case muzzley::JSDouble : {
			_out << this->__target.__double << flush;
			break;
		}
		case muzzley::JSBoolean : {
			_out << (this->__target.__boolean ? "true" : "false") << flush;
			break;
		}
		case muzzley::JSNil : {
			_out <<  "undefined" << flush;
			break;
		}
		case muzzley::JSDate : {
			string _date;
			muzzley::tostr(_date, this->__target.__date, "%Y-%m-%dT%H:%M:%S.000Z");
			_out << "\"" << _date << "\"" << flush;
			break;
		}
	}
}

void muzzley::JSONElementT::prettify(string& _out, uint _n_tabs) {
	switch(this->__target.__type) {
		case muzzley::JSObject : {
			this->__target.__object->prettify(_out, _n_tabs);
			break;
		}
		case muzzley::JSArray : {
			this->__target.__array->prettify(_out, _n_tabs);
			break;
		}
		case muzzley::JSString : {
			string _str(this->str());
			muzzley::replace(_str, "\"", "\\\"");
			muzzley::replace(_str, "\n", "\\\n");
			muzzley::replace(_str, "\r", "\\\b");
			muzzley::replace(_str, "\t", "\\\t");
			_out.insert(_out.length(), "\"");
			_out.insert(_out.length(), _str);
			_out.insert(_out.length(), "\"");
			break;
		}
		case muzzley::JSInteger : {
			muzzley::tostr(_out, this->__target.__integer);
			break;
		}
		case muzzley::JSDouble : {
			muzzley::tostr(_out, this->__target.__double);
			break;
		}
		case muzzley::JSBoolean : {
			muzzley::tostr(_out, this->__target.__boolean);
			break;
		}
		case muzzley::JSNil : {
			_out.insert(_out.length(), "undefined");
			break;
		}
		case muzzley::JSDate : {
			_out.insert(_out.length(), "\"");
			muzzley::tostr(_out, this->__target.__date, "%Y-%m-%dT%H-%M-%S.000Z");
			_out.insert(_out.length(), "\"");
			break;
		}
	}
}


/*JSON OBJECT*/
muzzley::JSONObjT::JSONObjT() {
}

muzzley::JSONObjT::~JSONObjT(){
}

void muzzley::JSONObjT::push(string _name) {
	if (this->__name.length() == 0) {
		this->__name.assign(_name);
	}
	else {
		this->pop(this->__name);
		this->insert(pair<string, JSONPtr>(string(this->__name.data()), JSONPtr(new JSONElementT(_name))));
		this->__name.clear();
	}
}

void muzzley::JSONObjT::push(JSONElementT& _value) {
	assertz(this->__name.length() != 0, "you must pass a field name first", 0, 0);
	this->pop(this->__name);
	this->insert(pair<string, JSONPtr>(this->__name, JSONPtr(new JSONElementT(_value))));
	this->__name.clear();
}

void muzzley::JSONObjT::push(JSONElementT* _value) {
	assertz(this->__name.length() != 0, "you must pass a field name first", 0, 0);
	this->pop(this->__name);
	this->insert(pair<string, JSONPtr>(this->__name, JSONPtr(_value)));
	this->__name.clear();
}

void muzzley::JSONObjT::pop(int _name) {
	this->pop(to_string(_name));
}

void muzzley::JSONObjT::pop(size_t _name) {
	this->pop(to_string(_name));
}

void muzzley::JSONObjT::pop(const char* _name) {
	this->pop(string(_name));
}

void muzzley::JSONObjT::pop(string _name) {
	auto _found = this->find(_name);
	if (_found != this->end()) {
		this->erase(_found);
	}
}

bool muzzley::JSONObjT::operator==(muzzley::JSONObjT& _rhs) {
	for (auto _f : * this) {
		auto _found = _rhs.find(_f.first);
		if (_found == _rhs.end()) {
			return false;
		}
		if (_found->second == _f.second) {
			continue;
		}
		return false;
	}
	return true;
}

bool muzzley::JSONObjT::operator==(muzzley::JSONObj& _rhs) {
	return * this == * _rhs;
}

bool muzzley::JSONObjT::operator!=(JSONObjT& _rhs) {
	for (auto _f : * this) {
		auto _found = _rhs.find(_f.first);
		if (_found == _rhs.end()) {
			return true;
		}
		if (_found->second != _f.second) {
			return true;
		}
	}
	return false;
}

bool muzzley::JSONObjT::operator!=(muzzley::JSONObj& _rhs) {
	return * this != * _rhs;
}

muzzley::JSONPtr& muzzley::JSONObjT::operator[](int _idx) {
	return (* this)[(size_t) _idx];
}

muzzley::JSONPtr& muzzley::JSONObjT::operator[](size_t _idx) {
	return (* this)[to_string(_idx)];
}

muzzley::JSONPtr& muzzley::JSONObjT::operator[](const char* _idx) {
	return (* this)[string(_idx)];
}

muzzley::JSONPtr& muzzley::JSONObjT::operator[](string _idx) {
	auto _found = this->find(_idx);
	if (_found != this->end()) {
		return _found->second;
	}
	return muzzley::undefined;
}

void muzzley::JSONObjT::stringify(string& _out) {
	_out.insert(_out.length(), "{");
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out.insert(_out.length(), ", ");
		}
		_first = false;
		_out.insert(_out.length(), "\"");
		_out.insert(_out.length(), _i.first);
		_out.insert(_out.length(), "\" : ");
		_i.second->stringify(_out);
	}
	_out.insert(_out.length(), "}");
}

void muzzley::JSONObjT::stringify(ostream& _out) {
	_out << "{" << flush;
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out << ", ";
		}
		_first = false;
		_out << "\"" << _i.first << "\" : " << flush;
		_i.second->stringify(_out);
	}
	_out << "}" << flush;
}

void muzzley::JSONObjT::prettify(string& _out, uint _n_tabs) {
	_out.insert(_out.length(), "{\n");
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out.insert(_out.length(), ",\n");
		}
		_first = false;
		_out.insert(_out.length(), string(_n_tabs + 1, '\t'));
		_out.insert(_out.length(), "\"");
		_out.insert(_out.length(), _i.first);
		_out.insert(_out.length(), "\" : ");
		_i.second->prettify(_out, _n_tabs + 1);
	}
	_out.insert(_out.length(), "\n");
	_out.insert(_out.length(), string(_n_tabs + 1, '\t'));
	_out.insert(_out.length(), "}");
}

void muzzley::JSONObjT::prettify(ostream& _out, uint _n_tabs) {
	_out << "{\n" << flush;
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out << ",\n ";
		}
		_first = false;
		_out << string(_n_tabs + 1, '\t') << "\"" << _i.first << "\" : " << flush;
		_i.second->prettify(_out, _n_tabs + 1);
	}
	_out << "\n" << string(_n_tabs, '\t') << "}" << flush;
}

/*JSON ARRAY*/
muzzley::JSONArrT::JSONArrT() {
}

muzzley::JSONArrT::~JSONArrT(){
}

void muzzley::JSONArrT::push(JSONElementT& _value) {
	this->push_back(JSONPtr(new JSONElementT(_value)));
}

void muzzley::JSONArrT::push(JSONElementT* _value) {
	this->push_back(JSONPtr(_value));
}

void muzzley::JSONArrT::pop(int _idx) {
	this->pop((size_t) _idx);
}

void muzzley::JSONArrT::pop(const char* _idx) {
	this->pop(string(_idx));
}

void muzzley::JSONArrT::pop(string _idx) {
	size_t _i = 0;
	muzzley::fromstr(_idx, &_i);

	assertz(_i >= 0, "the index of the element you want to remove must be higher then 0", 0, 0);
	assertz(_i < this->size(), "the index of the element you want to remove must be lower than the array size", 0, 0);
	this->erase(this->begin() + _i);
}

void muzzley::JSONArrT::pop(size_t _idx) {
	assertz(_idx >= 0, "the index of the element you want to remove must be higher then 0", 0, 0);
	assertz(_idx < this->size(), "the index of the element you want to remove must be lower than the array size", 0, 0);
	this->erase(this->begin() + _idx);
}

bool muzzley::JSONArrT::operator==(muzzley::JSONArrT& _rhs) {
	for (size_t _f  = 0; _f != this->size(); _f++) {
		if ((* this)[_f]  == _rhs[_f]) {
			continue;
		}
		return false;
	}
	return true;
}

bool muzzley::JSONArrT::operator==(muzzley::JSONArr& _rhs) {
	return * this == * _rhs;
}

bool muzzley::JSONArrT::operator!=(JSONArrT& _rhs) {
	for (size_t _f  = 0; _f != this->size(); _f++) {
		if ((* this)[_f]  != _rhs[_f]) {
			return true;
		}
	}
	return false;
}

bool muzzley::JSONArrT::operator!=(muzzley::JSONArr& _rhs) {
	return * this != * _rhs;
}

muzzley::JSONPtr& muzzley::JSONArrT::operator[](int _idx) {
	return (* this)[(size_t) _idx];
}

muzzley::JSONPtr& muzzley::JSONArrT::operator[](size_t _idx) {
	if (_idx < 0 ||_idx >= this->size()) {
		return muzzley::undefined;
	}
	return this->at(_idx);
}

muzzley::JSONPtr& muzzley::JSONArrT::operator[](const char* _idx) {
	return (* this)[string(_idx)];
}

muzzley::JSONPtr& muzzley::JSONArrT::operator[](string _idx) {
	size_t _i = 0;
	muzzley::fromstr(_idx, &_i);

	if (_i < 0 ||_i >= this->size()) {
		return muzzley::undefined;
	}

	return this->at(_i);
}

void muzzley::JSONArrT::stringify(string& _out) {
	_out.insert(_out.length(), "[");
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out.insert(_out.length(), ", ");
		}
		_first = false;
		_i->stringify(_out);
	}
	_out.insert(_out.length(), "]");
}

void muzzley::JSONArrT::stringify(ostream& _out) {
	_out << "[" << flush;
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out << ", ";
		}
		_first = false;
		_i->stringify(_out);
	}
	_out << "]" << flush;
}

void muzzley::JSONArrT::prettify(string& _out, uint _n_tabs) {
	_out.insert(_out.length(), "[\n");
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out.insert(_out.length(), ",\n");
		}
		_first = false;
		_out.insert(_out.length(), string(_n_tabs + 1, '\t'));
		_i->prettify(_out, _n_tabs + 1);
	}
	_out.insert(_out.length(), "\n]");
	_out.insert(_out.length(), string(_n_tabs + 1, '\t'));
	_out.insert(_out.length(), "]");
}

void muzzley::JSONArrT::prettify(ostream& _out, uint _n_tabs) {
	_out << "[\n" << flush;
	bool _first = true;
	for (auto _i : * this) {
		if (!_first) {
			_out << ",\n ";
		}
		_first = false;
		_out << string(_n_tabs + 1, '\t')<< flush;
		_i->prettify(_out, _n_tabs + 1);
	}
	_out << "\n" << string(_n_tabs, '\t') << "]" << flush;
}

/*JSON POINTER TO ELEMENT*/
muzzley::JSONPtr::JSONPtr()  : shared_ptr<JSONElementT>(make_shared<JSONElementT>()) {
}

muzzley::JSONPtr::JSONPtr(JSONElementT* _target) : shared_ptr<JSONElementT>(_target) {
}

muzzley::JSONPtr::~JSONPtr(){
}

muzzley::JSONElementT& muzzley::JSONPtr::value() {
	if (this->get() == nullptr) {
		*(muzzley::undefined.get());
	}
	return *(this->get());
}

muzzley::JSONPtr::operator string() {
	if (this->get() == nullptr) {
		return "";
	}
	string _out;
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			this->get()->obj()->stringify(_out);
			break;
		}
		case muzzley::JSArray : {
			this->get()->arr()->stringify(_out);
			break;
		}
		case muzzley::JSString : {
			_out.assign(this->get()->str().data());
			break;
		}
		case muzzley::JSInteger : {
			muzzley::tostr(_out, this->get()->intr());
			break;
		}
		case muzzley::JSDouble : {
			muzzley::tostr(_out, this->get()->dbl());
			break;
		}
		case muzzley::JSBoolean : {
			muzzley::tostr(_out, this->get()->bln());
			break;
		}
		case muzzley::JSNil : {
			_out.assign("");
			break;
		}
		case muzzley::JSDate : {
			muzzley::tostr(_out, this->get()->date(), "%Y-%m-%dT%H-%M-%S.000Z");
			break;
		}
	}
	return _out;
}

muzzley::JSONPtr::operator bool() {
	if (this->get() == nullptr) {
		return false;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return true;
		}
		case muzzley::JSArray : {
			return true;
		}
		case muzzley::JSString : {
			return this->get()->str().length() != 0;
		}
		case muzzley::JSInteger : {
			return (bool) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (bool) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return this->get()->bln();
		}
		case muzzley::JSNil : {
			return false;
		}
		case muzzley::JSDate : {
			return (bool) this->get()->date();
		}
	}
	return false;
}

muzzley::JSONPtr::operator int() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			int _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (int) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (int) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (int) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (int) this->get()->date();
		}
	}
	return 0;
}

muzzley::JSONPtr::operator long() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			long _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (long) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (long) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (long) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (long) this->get()->date();
		}
	}
	return 0;
}

muzzley::JSONPtr::operator long long() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			long long _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (long long) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (long long) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (long long) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (long long) this->get()->date();
		}
	}
	return 0;
}

#ifdef __LP64__
muzzley::JSONPtr::operator unsigned int() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			unsigned int _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (unsigned int) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (unsigned int) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (unsigned int) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (unsigned int) this->get()->date();
		}
	}
	return 0;
}
#endif

muzzley::JSONPtr::operator size_t() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			size_t _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (size_t) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (size_t) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (size_t) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (size_t) this->get()->date();
		}
	}
	return 0;
}

muzzley::JSONPtr::operator double() {
	if (this->get() == nullptr) {
		return 0;
	}
	switch(this->get()->type()) {
		case muzzley::JSObject : {
			return (double) this->get()->obj()->size();;
		}
		case muzzley::JSArray : {
			return (double) this->get()->arr()->size();;
		}
		case muzzley::JSString : {
			double _n = 0;
			string _s(this->get()->str().data());
			muzzley::fromstr(_s, &_n);
			return _n;
		}
		case muzzley::JSInteger : {
			return (double) this->get()->intr();
		}
		case muzzley::JSDouble : {
			return (double) this->get()->dbl();
		}
		case muzzley::JSBoolean : {
			return (double) this->get()->bln();
		}
		case muzzley::JSNil : {
			return 0;
		}
		case muzzley::JSDate : {
			return (double) this->get()->date();
		}
	}
	return 0;
}

muzzley::JSONPtr::operator JSONObj() {
	assertz(this->get() != nullptr && this->get()->type() == muzzley::JSObject, "this element is not of type JSObject", 0, 0);
	return this->get()->obj();
}

muzzley::JSONPtr::operator JSONArr() {
	assertz(this->get() != nullptr && this->get()->type() == muzzley::JSArray, "this element is not of type JSArray", 0, 0);
	return this->get()->arr();
}

muzzley::JSONPtr::operator JSONObj&() {
	assertz(this->get() != nullptr && this->get()->type() == muzzley::JSObject, "this element is not of type JSObject", 0, 0);
	return this->get()->obj();
}

muzzley::JSONPtr::operator JSONArr&() {
	assertz(this->get() != nullptr && this->get()->type() == muzzley::JSArray, "this element is not of type JSArray", 0, 0);
	return this->get()->arr();
}

void muzzley::JSONPtr::parse(istream& _in) {
	muzzley::JSONParser _p;
	_p.switchRoots(* this);
	_p.switchStreams(_in);
	_p.parse();
}

/*JSON POINTER TO OBJECT*/
muzzley::JSONObj::JSONObj() : shared_ptr<JSONObjT>(make_shared<JSONObjT>(JSONObjT())) {
}

muzzley::JSONObj::JSONObj(JSONObj& _rhs)  : shared_ptr<JSONObjT>(_rhs) {
}

muzzley::JSONObj::JSONObj(JSONObjT* _target) : shared_ptr<JSONObjT>(_target) {
}

muzzley::JSONObj::~JSONObj(){
}

muzzley::JSONObjT::iterator muzzley::JSONObj::begin() {
	return (* this)->begin();
}

muzzley::JSONObjT::iterator muzzley::JSONObj::end() {
	return (* this)->end();
}

muzzley::JSONObj& muzzley::JSONObj::operator<<(string _in) {
	(* this)->push(_in);
	return * this;
}

muzzley::JSONObj& muzzley::JSONObj::operator<<(const char* _in) {
	(* this)->push(_in);
	return * this;
}

/*JSON POINTER TO ARRAY*/
muzzley::JSONArr::JSONArr() : shared_ptr<JSONArrT>(make_shared<JSONArrT>(JSONArrT())) {
}

muzzley::JSONArr::JSONArr(JSONArr& _rhs)  : shared_ptr<JSONArrT>(_rhs){
}

muzzley::JSONArr::JSONArr(JSONArrT* _target) : shared_ptr<JSONArrT>(_target) {
}

muzzley::JSONArr::~JSONArr(){
}

muzzley::JSONArrT::iterator muzzley::JSONArr::begin() {
	return (* this)->begin();
}

muzzley::JSONArrT::iterator muzzley::JSONArr::end() {
	return (* this)->end();
}
