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
#include <muzzley/config.h>
#ifdef HAVE_HTTP_CLIENT

#include <muzzley/http/HTTPObj.h>

#include <iostream>
#include <muzzley/parsers/HTTPParser.h>
#include <muzzley/exceptions/CastException.h>
#include <muzzley/exceptions/NoHeaderNameException.h>

muzzley::HTTPReqT::HTTPReqT() : __method(muzzley::HTTPGet) {
}

muzzley::HTTPReqT::~HTTPReqT() {
}

muzzley::HTTPMethod muzzley::HTTPReqT::method() {
	return this->__method;
}

void muzzley::HTTPReqT::method(muzzley::HTTPMethod _method) {
	this->__method = _method;
}

string& muzzley::HTTPReqT::url() {
	return this->__url;
}

void muzzley::HTTPReqT::url(string _url) {
	this->__url.assign(_url.data());
}

string& muzzley::HTTPReqT::query() {
	return this->__query;
}

void muzzley::HTTPReqT::query(string _query) {
	this->__query.assign(_query.data());
}

muzzley::ParameterMap& muzzley::HTTPReqT::params() {
	return this->__params;
}

string muzzley::HTTPReqT::param(const char* _idx) {
	auto _found = this->__params.find(_idx);
	if (_found != this->__params.end()) {
		return _found->second;
	}
	return "";
}

void muzzley::HTTPReqT::param(const char* _name, const char* _value) {
	this->__params.insert(pair< string, string> (_name, _value));
}

void muzzley::HTTPReqT::param(const char* _name, string _value) {
	this->__params.insert(pair< string, string> (_name, _value));
}

void muzzley::HTTPReqT::param(string _name, string _value) {
	this->__params.insert(pair< string, string> (_name, _value));
}

void muzzley::HTTPReqT::stringify(ostream& _out) {
	string _ret;
	this->stringify(_ret);
	_out << _ret << flush;
}

void muzzley::HTTPReqT::stringify(string& _out) {
	_out.insert(_out.length(), muzzley::method_names[this->__method]);
	_out.insert(_out.length(),  " ");
	_out.insert(_out.length(), this->__url);
	if (this->__params.size() != 0) {
		_out.insert(_out.length(), "?");
		bool _first = true;
		for (auto i : this->__params) {
			if (!_first) {
				_out.insert(_out.length(), "&");
			}
			_first = false;
			string _n(i.first);
			muzzley::url_encode(_n);
			string _v(i.second);
			muzzley::url_encode(_v);
			_out.insert(_out.length(), _n);
			_out.insert(_out.length(), "=");
			_out.insert(_out.length(), _v);
		}
	}
	_out.insert(_out.length(), " HTTP/1.1");
	_out.insert(_out.length(),  CRLF);
	for (auto h : this->__headers) {
		_out.insert(_out.length(), h.first);
		_out.insert(_out.length(), ": ");
		_out.insert(_out.length(), h.second);
		_out.insert(_out.length(), CRLF);
	}
	_out.insert(_out.length(), CRLF);
	_out.insert(_out.length(), this->__body);
}

muzzley::HTTPReq::HTTPReq()  : shared_ptr<HTTPReqT>(make_shared<HTTPReqT>()) {
}

muzzley::HTTPReq::HTTPReq(HTTPReqT* _target) : shared_ptr<HTTPReqT>(_target) {
}

muzzley::HTTPReq::~HTTPReq(){
}

void muzzley::HTTPReq::parse(istream& _in){
	muzzley::HTTPParser _p;
	_p.switchRoots(* this);
	_p.switchStreams(_in);
	_p.parse();
}

#endif