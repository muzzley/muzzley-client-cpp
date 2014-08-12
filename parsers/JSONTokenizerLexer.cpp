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


#include <parsers/JSONTokenizerLexer.h>

muzzley::JSONTokenizerLexer::JSONTokenizerLexer(std::istream &_in, std::ostream &_out, muzzley::JSONObj* _rootobj, muzzley::JSONArr* _rootarr) :
	muzzley::JSONLexer(_in, _out), __root_obj(_rootobj), __root_arr(_rootarr), __root_type(muzzley::JSObject), __value(NULL), __parent(NULL) {
}

muzzley::JSONTokenizerLexer::~JSONTokenizerLexer() {
}

void muzzley::JSONTokenizerLexer::switchRoots(JSONObj* _rootobj, JSONArr* _rootarr) {
	this->__root_obj = _rootobj;
	this->__root_arr = _rootarr;
}

void muzzley::JSONTokenizerLexer::result(muzzley::JSONType _in) {
	this->__root_type = _in;
	switch (_in) {
		case muzzley::JSObject : {
			this->__root_obj->set((JSONObjRef*) this->__parent);
			break;
		}
		case muzzley::JSArray : {
			this->__root_arr->set((JSONArrRef*) this->__parent);
			break;
		}
		default: {
		}
	}
	this->__parent = NULL;
	this->__value = NULL;
}

void muzzley::JSONTokenizerLexer::finish(muzzley::JSONType _in) {
	this->__value = this->__parent;
	if (this->__context.size() != 0) {
		this->__parent = this->__context.back();
		this->__context.pop_back();
	}
}

void muzzley::JSONTokenizerLexer::init(muzzley::JSONType _in_type, const string _in_str) {
	(*this->__parent) << _in_str;
}

void muzzley::JSONTokenizerLexer::init(muzzley::JSONType _in_type) {
	if (this->__parent != NULL) {
		this->__context.push_back(this->__parent);
		this->__parent = NULL;
	}
	switch (_in_type) {
		case muzzley::JSObject : {
			this->__parent = new JSONObjRef();
			break;
		}
		case muzzley::JSArray : {
			this->__parent = new JSONArrRef();
			break;
		}
		default : {
		}
	}
}

void muzzley::JSONTokenizerLexer::init(bool _in) {
	this->__value = new JSONBoolRef(_in);
}

void muzzley::JSONTokenizerLexer::init(long long _in) {
	this->__value = new JSONIntRef(_in);
}

void muzzley::JSONTokenizerLexer::init(double _in) {
	this->__value = new JSONDblRef(_in);
}

void muzzley::JSONTokenizerLexer::init(string _in) {
	this->__value = new JSONStrRef(_in);
}

void muzzley::JSONTokenizerLexer::add() {
	switch (this->__value->type()) {
		case muzzley::JSObject : {
			muzzley::JSONObj _obj((JSONObjRef*) this->__value);
			(*this->__parent) << _obj;
			break;
		}
		case muzzley::JSArray : {
			muzzley::JSONArr _arr((JSONArrRef*) this->__value);
			(*this->__parent) << _arr;
			break;
		}
		case muzzley::JSBoolean : {
			(*this->__parent) << (bool) *this->__value;
			delete this->__value;
			break;
		}
		case muzzley::JSInteger : {
			(*this->__parent) << (long long) *this->__value;
			delete this->__value;
			break;
		}
		case muzzley::JSDouble : {
			(*this->__parent) << (double) *this->__value;
			delete this->__value;
			break;
		}
		case muzzley::JSString : {
			(*this->__parent) << (string) *this->__value;
			delete this->__value;
			break;
		}
		case muzzley::JSNil : {
			(*this->__parent) << muzzley::undefined;
			delete this->__value;
			break;
		}
		default: {
		}
	}
	this->__value = NULL;
}

