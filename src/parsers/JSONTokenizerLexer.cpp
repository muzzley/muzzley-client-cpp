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


#include <muzzley/parsers/JSONTokenizerLexer.h>

muzzley::JSONTokenizerLexer::JSONTokenizerLexer(std::istream &_in, std::ostream &_out) : muzzley::JSONLexer(_in, _out) {
	this->__root = this->__parent = nullptr;
}

muzzley::JSONTokenizerLexer::~JSONTokenizerLexer() {
}

void muzzley::JSONTokenizerLexer::switchRoots(JSONPtr& _root) {
	this->__root = this->__parent = _root.get();
}

void muzzley::JSONTokenizerLexer::result(muzzley::JSONType _in) {
	try {			
		this->__root_type = _in;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::finish(muzzley::JSONType _in) {
	try {			
		//cout << "- finishing object: " << (* this->__parent) <<  " > " << this->__parent->type() << endl << flush;
		this->__parent = this->__parent->parent();
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::init(muzzley::JSONType _in_type, const string _in_str) {
	try {			
		//cout << "- starting field: " << _in_str << endl << flush;
		(* this->__parent) << _in_str;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::init(muzzley::JSONType _in_type) {
	try {		
		//cout << "- putting this object on hold: " << (* this->__parent) <<  " > " << this->__parent->type() << endl << flush;
		switch (_in_type) {
			case muzzley::JSObject : {
				JSONObj _obj;
				JSONElementT* _ptr = new JSONElementT(_obj);
				_ptr->parent(this->__parent);
				(* this->__parent) << _ptr;
				this->__parent = _ptr;
				//cout << "- starting object: " << (* this->__parent) <<  " > " << this->__parent->type() << " with parent " << this->__parent->parent() << endl << flush;
				break;
			}
			case muzzley::JSArray : {
				JSONArr _arr;
				JSONElementT* _ptr = new JSONElementT(_arr);
				_ptr->parent(this->__parent);
				(* this->__parent) << _ptr;
				this->__parent = _ptr;
				//cout << "- starting object: " << (* this->__parent) <<  " > " << this->__parent->type() << " with parent " << this->__parent->parent() << endl << flush;
				break;
			}
			default : {
			}
		}
	}
	catch (muzzley::AssertionException& _e) {
		this->__parent->type(_in_type);
	}
}

void muzzley::JSONTokenizerLexer::init(bool _in) {
	try {			
		//cout << "- adding value: " << _in << endl << flush;
		JSONElementT* _ptr = new JSONElementT(_in);
		_ptr->parent(this->__parent);
		(* this->__parent) << _ptr;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::init(long long _in) {
	try {			
		//cout << "- adding value: " << _in << endl << flush;
		JSONElementT* _ptr = new JSONElementT(_in);
		_ptr->parent(this->__parent);
		(* this->__parent) <<  _ptr;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::init(double _in) {
	try {			
		//cout << "- adding value: " << _in << endl << flush;
		JSONElementT* _ptr = new JSONElementT(_in);
		_ptr->parent(this->__parent);
		(* this->__parent) <<  _ptr;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::init(string _in) {
	try {			
		//cout << "- adding value: " << _in << endl << flush;
		JSONElementT* _ptr = new JSONElementT(_in);
		_ptr->parent(this->__parent);
		(* this->__parent) <<  _ptr;
	}
	catch (muzzley::AssertionException& _e) {
		cout << __FILE__ << ":" << __LINE__ << " " << _e.description() << endl << flush;
		throw _e;
	}
}

void muzzley::JSONTokenizerLexer::add() {
}

