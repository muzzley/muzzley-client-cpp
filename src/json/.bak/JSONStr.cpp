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

#include <iostream>
#include <muzzley/text/convert.h>
#include <muzzley/exceptions/CastException.h>
#include <muzzley/exceptions/NoAttributeNameException.h>

muzzley::JSONStrRef::JSONStrRef(string _in) : __value(_in) {
}

muzzley::JSONStrRef::~JSONStrRef() {
}

muzzley::JSONType muzzley::JSONStrRef::type() {
	return muzzley::JSString;
}

void muzzley::JSONStrRef::put(int _in) {
	muzzley::tostr(this->__value, _in);
}

void muzzley::JSONStrRef::put(long _in) {
	muzzley::tostr(this->__value, _in);
}

void muzzley::JSONStrRef::put(long long _in) {
	muzzley::tostr(this->__value, _in);
}

#ifdef __LP64__
void muzzley::JSONStrRef::put(unsigned int _in) {
	muzzley::tostr(this->__value, _in);
}
#endif

void muzzley::JSONStrRef::put(size_t _in) {
	muzzley::tostr(this->__value, _in);
}

void muzzley::JSONStrRef::put(double _in) {
	muzzley::tostr(this->__value, _in);
}

void muzzley::JSONStrRef::put(bool _in) {
	muzzley::tostr(this->__value, _in);
}

void muzzley::JSONStrRef::put(string _in) {
	this->__value.assign(_in);
}

void muzzley::JSONStrRef::put(JSONObj& _in) {
	_in.get()->stringify(this->__value);
}

void muzzley::JSONStrRef::put(JSONArr& _in) {
	_in.get()->stringify(this->__value);
}

void muzzley::JSONStrRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONStrRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONStrRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONStrRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONStrRef::put(JSONNil& _in) {
	this->__value.assign("null");
}

bool muzzley::JSONStrRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this->__value == ((JSONStrRef) _in).__value;
}

muzzley::JSONElement& muzzley::JSONStrRef::get(size_t _idx) {
	return *this;
}

muzzley::JSONElement& muzzley::JSONStrRef::get(const char* _idx) {
	return *this;
}

int muzzley::JSONStrRef::getInt() {
	int _ret = 0;
	muzzley::fromstr(this->__value, &_ret);
	return _ret;
}

long muzzley::JSONStrRef::getLong() {
	long _ret = 0;
	muzzley::fromstr(this->__value, &_ret);
	return _ret;
}

long muzzley::JSONStrRef::getLongLong() {
	long long _ret = 0;
	muzzley::fromstr(this->__value, &_ret);
	return _ret;
}

unsigned int muzzley::JSONStrRef::getUnsignedInt() {
	unsigned int _ret = 0;
	muzzley::fromstr(this->__value, &_ret);
	return _ret;
}

double muzzley::JSONStrRef::getDouble() {
	double _ret = 0;
	muzzley::fromstr(this->__value, &_ret);
	return _ret;
}

bool muzzley::JSONStrRef::getBool() {
	return this->__value.length() != 0;
}

string muzzley::JSONStrRef::getString() {
	return this->__value;
}

muzzley::JSONObjRef& muzzley::JSONStrRef::getJSONObj() {
	throw CastException("can not convert from basic type to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONStrRef::getJSONArr() {
	throw CastException("can not convert from basic type to JSONArr");
}

void muzzley::JSONStrRef::stringify(ostream& _out, short _flags, string _tabs) {
	string encoded(this->__value);
	muzzley::utf8_encode(encoded);
	_out << "\"" << encoded << "\"" << flush;
}

void muzzley::JSONStrRef::stringify(string& _out, short _flags, string _tabs) {
	string encoded(this->__value);
	muzzley::utf8_encode(encoded);
	_out.insert(_out.length(),  "\"");
	_out.insert(_out.length(), encoded);
	_out.insert(_out.length(), "\"");
}
