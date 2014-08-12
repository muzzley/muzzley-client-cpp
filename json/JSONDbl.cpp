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

muzzley::JSONDblRef::JSONDblRef(double _in) : __value(_in) {
}

muzzley::JSONDblRef::~JSONDblRef() {
}

muzzley::JSONType muzzley::JSONDblRef::type() {
	return muzzley::JSDouble;
}

void muzzley::JSONDblRef::put(int _in) {
	this->__value = _in;
}

void muzzley::JSONDblRef::put(long _in) {
	this->__value = _in;
}

void muzzley::JSONDblRef::put(long long _in) {
	this->__value = _in;
}

#ifdef __LP64__
void muzzley::JSONDblRef::put(unsigned int _in) {
	this->__value = _in;
}
#endif

void muzzley::JSONDblRef::put(size_t _in) {
	this->__value = _in;
}

void muzzley::JSONDblRef::put(double _in) {
	this->__value = _in;
}

void muzzley::JSONDblRef::put(bool _in) {
	this->__value = _in;
}

void muzzley::JSONDblRef::put(string _in) {
	muzzley::fromstr(_in, &this->__value);
}

void muzzley::JSONDblRef::put(JSONObj& _in) {
}

void muzzley::JSONDblRef::put(JSONArr& _in) {
}

void muzzley::JSONDblRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void muzzley::JSONDblRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void muzzley::JSONDblRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void muzzley::JSONDblRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void muzzley::JSONDblRef::put(JSONNil& _in) {
	this->__value = 0;
}

bool muzzley::JSONDblRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this->__value == ((JSONDblRef) _in).__value;
}

muzzley::JSONElement& muzzley::JSONDblRef::get(size_t _idx) {
	return *this;
}

muzzley::JSONElement& muzzley::JSONDblRef::get(const char* _idx) {
	return *this;
}

int muzzley::JSONDblRef::getInt() {
	return (int) this->__value;
}

long muzzley::JSONDblRef::getLong() {
	return (long) this->__value;
}

long muzzley::JSONDblRef::getLongLong() {
	return (long long) this->__value;
}

unsigned int muzzley::JSONDblRef::getUnsignedInt() {
	return (unsigned int) this->__value;
}

double muzzley::JSONDblRef::getDouble() {
	return this->__value;
}

bool muzzley::JSONDblRef::getBool() {
	return (bool) this->__value;
}

string muzzley::JSONDblRef::getString() {
	string _ret;
	muzzley::tostr(_ret, this->__value);
	return _ret;
}

muzzley::JSONObjRef& muzzley::JSONDblRef::getJSONObj() {
	throw CastException("can not convert from basic type to JSONObj");
}

muzzley::JSONArrRef& muzzley::JSONDblRef::getJSONArr() {
	throw CastException("can not convert from basic type to JSONArr");
}

void muzzley::JSONDblRef::stringify(ostream& _out, short _flags, string _tabs) {
	_out << this->__value << flush;
}

void muzzley::JSONDblRef::stringify(string& _out, short _flags, string _tabs) {
	muzzley::tostr(_out, this->__value);
}
