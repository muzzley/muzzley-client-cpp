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
#include <text/convert.h>
#include <exceptions/CastException.h>
#include <exceptions/NoAttributeNameException.h>

muzzley::JSONIntRef::JSONIntRef(long long _in) : __value(_in) {
}

muzzley::JSONIntRef::~JSONIntRef() {
}

muzzley::JSONType muzzley::JSONIntRef::type() {
	return muzzley::JSInteger;
}

void muzzley::JSONIntRef::put(int _in) {
	this->__value = _in;
}

void muzzley::JSONIntRef::put(long _in) {
	this->__value = _in;
}

void muzzley::JSONIntRef::put(long long _in) {
	this->__value = _in;
}

#ifdef __LP64__
void muzzley::JSONIntRef::put(unsigned int _in) {
	this->__value = _in;
}
#endif

void muzzley::JSONIntRef::put(size_t _in) {
	this->__value = _in;
}

void muzzley::JSONIntRef::put(double _in) {
	this->__value = _in;
}

void muzzley::JSONIntRef::put(bool _in) {
	this->__value = _in;
}

void muzzley::JSONIntRef::put(string _in) {
	muzzley::fromstr(_in, &this->__value);
}

void muzzley::JSONIntRef::put(JSONObj& _in) {
}

void muzzley::JSONIntRef::put(JSONArr& _in) {
}

void muzzley::JSONIntRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONIntRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONIntRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONIntRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONIntRef::put(JSONNil& _in) {
	this->__value = 0;
}

bool muzzley::JSONIntRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this->__value == ((JSONIntRef) _in).__value;
}

muzzley::JSONElement& muzzley::JSONIntRef::get(size_t _idx) {
	return *this;
}

muzzley::JSONElement& muzzley::JSONIntRef::get(const char* _idx) {
	return *this;
}

int muzzley::JSONIntRef::getInt() {
	return (int) this->__value;
}

long muzzley::JSONIntRef::getLong() {
	return (long) this->__value;
}

long muzzley::JSONIntRef::getLongLong() {
	return (long long) this->__value;
}

unsigned int muzzley::JSONIntRef::getUnsignedInt() {
	return (unsigned int) this->__value;
}

double muzzley::JSONIntRef::getDouble() {
	return (double) this->__value;
}

bool muzzley::JSONIntRef::getBool() {
	return (bool) this->__value;
}

string muzzley::JSONIntRef::getString() {
	string _ret;
	muzzley::tostr(_ret, this->__value);
	return _ret;
}

muzzley::JSONObjRef& muzzley::JSONIntRef::getJSONObj() {
	throw CastException("can not convert from basic type to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONIntRef::getJSONArr() {
	throw CastException("can not convert from basic type to JSONArr");
}

void muzzley::JSONIntRef::stringify(ostream& _out, short _flags, string _tabs) {
	_out << this->__value << flush;
}

void muzzley::JSONIntRef::stringify(string& _out, short _flags, string _tabs) {
	muzzley::tostr(_out, this->__value);
}

