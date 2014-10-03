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

namespace muzzley {
	JSONElement& nil = *(new muzzley::JSONElement());
}

muzzley::JSONElement::JSONElement() {
	this->__flags = 0;
}

muzzley::JSONElement::~JSONElement() {
}

muzzley::JSONType muzzley::JSONElement::type() {
	return muzzley::JSNil;
}

short muzzley::JSONElement::flags() {
	return this->__flags;
}

void muzzley::JSONElement::put(int _in) {
}

void muzzley::JSONElement::put(long _in) {
}

void muzzley::JSONElement::put(long long _in) {
}

#ifdef __LP64__
void muzzley::JSONElement::put(unsigned int _in) {
}
#endif

void muzzley::JSONElement::put(size_t _in) {
}

void muzzley::JSONElement::put(double _in) {
}

void muzzley::JSONElement::put(bool _in) {
}

void muzzley::JSONElement::put(string _in) {
}

void muzzley::JSONElement::put(ObjectOp _in) {
	if (_in == muzzley::undefined) {
		muzzley::JSONNil _in;
		this->put(_in);
	}
	this->__flags |= _in;
}

void muzzley::JSONElement::unset(string _in) {
}

void muzzley::JSONElement::unset(long long _in) {
}

void muzzley::JSONElement::unset(ObjectOp _in) {
	this->__flags ^= _in;
}

void muzzley::JSONElement::put(JSONObj& _in) {
}

void muzzley::JSONElement::put(JSONArr& _in) {
}

void muzzley::JSONElement::put(JSONBool& _in) {
}

void muzzley::JSONElement::put(JSONInt& _in) {
}

void muzzley::JSONElement::put(JSONDbl& _in) {
}

void muzzley::JSONElement::put(JSONStr& _in) {
}

void muzzley::JSONElement::put(JSONNil& _in) {
}

bool muzzley::JSONElement::compare(JSONElement& _in) {
	return _in.type() == muzzley::JSNil;
}

muzzley::JSONElement& muzzley::JSONElement::get(size_t _idx) {
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::get(const char* _idx) {
	return *this;
}

int muzzley::JSONElement::getInt() {
	return 0;
}

long muzzley::JSONElement::getLong() {
	return 0;
}

long muzzley::JSONElement::getLongLong() {
	return 0;
}

unsigned int muzzley::JSONElement::getUnsignedInt() {
	return 0;
}

double muzzley::JSONElement::getDouble() {
	return 0;
}

bool muzzley::JSONElement::getBool() {
	return false;
}

string muzzley::JSONElement::getString() {
	return "";
}

muzzley::JSONObjRef& muzzley::JSONElement::getJSONObj() {
	throw CastException("can not convert from generic type to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONElement::getJSONArr() {
	throw CastException("can not convert from generic type to JSONArr");
}

muzzley::JSONElement& muzzley::JSONElement::getJSONElement() {
	return *this;
}

void muzzley::JSONElement::stringify(ostream& _out, short _flags, string _tabs) {
	_out << "null" << flush;
}

void muzzley::JSONElement::stringify(string& _out, short _flags, string _tabs) {
	_out.insert(_out.length(), "null");
}

bool muzzley::JSONElement::operator==(JSONElement& _in) {
	return this->compare(_in);
}

bool muzzley::JSONElement::operator==(string _in) {
	return ((string) *this) == _in;
}

bool muzzley::JSONElement::operator==(bool _in) {
	return ((bool) *this) == _in;
}

bool muzzley::JSONElement::operator==(int _in) {
	return ((int) *this) == _in;
}

bool muzzley::JSONElement::operator==(long _in) {
	return ((long) *this) == _in;
}

bool muzzley::JSONElement::operator==(long long _in) {
	return ((long long) *this) == _in;
}

bool muzzley::JSONElement::operator==(double _in) {
	return ((double) *this) == _in;
}

bool muzzley::JSONElement::operator!=(JSONElement& _in) {
	return !(*this == _in);
}

bool muzzley::JSONElement::operator!=(string _in) {
	return ((string) *this) != _in;
}

bool muzzley::JSONElement::operator!=(bool _in) {
	return ((bool) *this) != _in;
}

bool muzzley::JSONElement::operator!=(int _in) {
	return ((int) *this) != _in;
}

bool muzzley::JSONElement::operator!=(long _in) {
	return ((long) *this) != _in;
}

bool muzzley::JSONElement::operator!=(long long _in) {
	return ((long long) *this) != _in;
}

bool muzzley::JSONElement::operator!=(double _in) {
	return ((double) *this) != _in;
}

muzzley::JSONElement::operator string() {
	return this->getString();
}

muzzley::JSONElement::operator bool() {
	return this->getBool();
}

muzzley::JSONElement::operator int() {
	return this->getInt();
}

muzzley::JSONElement::operator long() {
	return this->getLong();
}

muzzley::JSONElement::operator long long() {
	return this->getLongLong();
}

#ifdef __LP64__
muzzley::JSONElement::operator unsigned int() {
	return this->getUnsignedInt();
}
#endif

muzzley::JSONElement::operator size_t() {
	return this->getUnsignedInt();
}

muzzley::JSONElement::operator double() {
	return this->getDouble();
}

muzzley::JSONElement::operator muzzley::JSONObjRef&() {
	return this->getJSONObj();
}

muzzley::JSONElement::operator muzzley::JSONArrRef&() {
	return this->getJSONArr();
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(const char* _in) {
	this->put(string(_in));
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(string _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(bool _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(int _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(long _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(long long _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(double _in) {
	this->put(_in);
	return *this;
}

#ifdef __LP64__
muzzley::JSONElement& muzzley::JSONElement::operator <<(unsigned int _in) {
	this->put(_in);
	return *this;
}
#endif

muzzley::JSONElement& muzzley::JSONElement::operator <<(size_t _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(muzzley::ObjectOp _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONObj& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONArr& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONInt& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONBool& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONDbl& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONStr& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator<<(JSONNil& _in) {
	this->put(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator>>(const char* _in) {
	this->unset(string(_in));
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator>>(long long _in) {
	this->unset(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator>>(string _in) {
	this->unset(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator>>(muzzley::ObjectOp _in) {
	this->unset(_in);
	return *this;
}

muzzley::JSONElement& muzzley::JSONElement::operator[](int _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONElement::operator[](size_t _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONElement::operator[](const char* _idx) {
	return this->get(_idx);
}

muzzley::JSONElement& muzzley::JSONElement::operator[](string& _idx) {
	return this->get(_idx.data());
}
