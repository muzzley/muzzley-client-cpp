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

muzzley::JSONBoolRef::JSONBoolRef(bool _in) : __value(_in) {
}

muzzley::JSONBoolRef::~JSONBoolRef() {
}

muzzley::JSONType muzzley::JSONBoolRef::type() {
	return muzzley::JSBoolean;
}

void muzzley::JSONBoolRef::put(int _in) {
	this->__value = _in;
}

void muzzley::JSONBoolRef::put(long _in) {
	this->__value = _in;
}

void muzzley::JSONBoolRef::put(long long _in) {
	this->__value = _in;
}

#ifdef __LP64__
void muzzley::JSONBoolRef::put(unsigned int _in) {
	this->__value = _in;
}
#endif

void muzzley::JSONBoolRef::put(size_t _in) {
	this->__value = _in;
}

void muzzley::JSONBoolRef::put(double _in) {
	this->__value = _in;
}

void muzzley::JSONBoolRef::put(bool _in) {
	this->__value = _in;
}

void muzzley::JSONBoolRef::put(string _in) {
	this->__value = (_in != string(""));
}

void muzzley::JSONBoolRef::put(JSONObj& _in) {
}

void muzzley::JSONBoolRef::put(JSONArr& _in) {
}

void muzzley::JSONBoolRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONBoolRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONBoolRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONBoolRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONBoolRef::put(JSONNil& _in) {
	this->__value = 0;
}

bool muzzley::JSONBoolRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this->__value == ((JSONBoolRef) _in).__value;
}

muzzley::JSONElement& muzzley::JSONBoolRef::get(size_t _idx) {
	return *this;
}

muzzley::JSONElement& muzzley::JSONBoolRef::get(const char* _idx) {
	return *this;
}

int muzzley::JSONBoolRef::getInt() {
	return (int) this->__value;
}

long muzzley::JSONBoolRef::getLong() {
	return (long) this->__value;
}

long muzzley::JSONBoolRef::getLongLong() {
	return (long long) this->__value;
}

unsigned int muzzley::JSONBoolRef::getUnsignedInt() {
	return (unsigned int) this->__value;
}

double muzzley::JSONBoolRef::getDouble() {
	return (double) this->__value;
}

bool muzzley::JSONBoolRef::getBool() {
	return (bool) this->__value;
}

string muzzley::JSONBoolRef::getString() {
	string _ret;
	muzzley::tostr(_ret, (bool) this->__value);
	return _ret;
}

muzzley::JSONObjRef& muzzley::JSONBoolRef::getJSONObj() {
	throw CastException("can not convert from basic type to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONBoolRef::getJSONArr() {
	throw CastException("can not convert from basic type to JSONArr");
}

void muzzley::JSONBoolRef::stringify(ostream& _out, short _flags, string _tabs) {
	_out << (bool) this->__value << flush;
}

void muzzley::JSONBoolRef::stringify(string& _out, short _flags, string _tabs) {
	muzzley::tostr(_out, (bool) this->__value);
}
