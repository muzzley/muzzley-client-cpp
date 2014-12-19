/*
The MIT License (MIT)

Copyright (c) 2014 Pedro (n@zgul) Figueiredo <pedro.figueiredo@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef PACKAGE
#include <muzzley/config.h>
#endif
#ifdef HAVE_HTTP_CLIENT

#include <muzzley/parsers/HTTPTokenizerLexer.h>

muzzley::HTTPTokenizerLexer::HTTPTokenizerLexer(std::istream &_in, std::ostream &_out) :
	muzzley::HTTPLexer(_in, _out) {
}

muzzley::HTTPTokenizerLexer::~HTTPTokenizerLexer() {
}

void muzzley::HTTPTokenizerLexer::switchRoots(HTTPReq& _root) {
	this->__root_req = _root.get();
}

void muzzley::HTTPTokenizerLexer::switchRoots(HTTPRep& _root) {
	this->__root_rep = _root.get();
}

void muzzley::HTTPTokenizerLexer::justLeave() {
	this->leave(-1);
}

void muzzley::HTTPTokenizerLexer::init(muzzley::HTTPType _in_type) {
	this->d_chunked_body = false;
	this->d_chunked.clear();
	this->__root_type = _in_type;
	switch (_in_type) {
		case muzzley::HTTPRequest : {
			muzzley::HTTPMethod _m;
			string _ms(this->matched());
			muzzley::fromstr(_ms, &_m);
			this->__root_req->method(_m);
			break;
		}
		case muzzley::HTTPReply : {
			break;
		}
	}
}

void muzzley::HTTPTokenizerLexer::body() {
	switch (this->__root_type) {
		case muzzley::HTTPRequest : {
			this->__root_req->body(this->matched());
			break;
		}
		case muzzley::HTTPReply : {
			this->__root_rep->body(this->matched());
			break;
		}
	}
}

void muzzley::HTTPTokenizerLexer::url() {
	switch (this->__root_type) {
		case muzzley::HTTPRequest : {
			this->__root_req->url(this->matched());
			break;
		}
		case muzzley::HTTPReply : {
			break;
		}
	}
}

void muzzley::HTTPTokenizerLexer::status() {
	switch (this->__root_type) {
		case muzzley::HTTPRequest : {
			break;
		}
		case muzzley::HTTPReply : {
			int _status = 0;
			string _statusstr(this->matched());
			muzzley::fromstr(_statusstr, &_status);
			this->__root_rep->status((muzzley::HTTPStatus) _status);
			break;
		}
	}
}

void muzzley::HTTPTokenizerLexer::add() {
	string _s(this->matched());
	muzzley::trim(_s);
	if (this->__header_name.length() == 0) {
		this->__header_name.assign(_s);
		return;
	}
	switch (this->__root_type) {
		case muzzley::HTTPRequest : {
			this->__root_req->header(this->__header_name, _s);
			break;
		}
		case muzzley::HTTPReply : {
			this->__root_rep->header(this->__header_name, _s);
			break;
		}
	}
	this->__header_name.clear();
}

void muzzley::HTTPTokenizerLexer::name() {
	string _s(this->matched());
	muzzley::trim(_s);
	this->__param_name.assign(_s);			
}

void muzzley::HTTPTokenizerLexer::value() {
	string _s(this->matched());
	muzzley::trim(_s);
	switch (this->__root_type) {
		case muzzley::HTTPRequest : {
			this->__root_req->param(this->__param_name, _s);
			break;
		}
		case muzzley::HTTPReply : {
			break;
		}
	}
	this->__param_name.clear();
}

#endif