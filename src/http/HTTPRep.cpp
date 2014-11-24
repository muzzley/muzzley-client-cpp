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

#include <muzzley/http/HTTPObj.h>

#include <iostream>
#include <muzzley/parsers/HTTPParser.h>
#include <muzzley/exceptions/CastException.h>
#include <muzzley/exceptions/NoHeaderNameException.h>

muzzley::HTTPRepT::HTTPRepT() : __status(muzzley::HTTP100) {
}

muzzley::HTTPRepT::~HTTPRepT() {
}

muzzley::HTTPStatus muzzley::HTTPRepT::status() {
	return this->__status;
}

void muzzley::HTTPRepT::status(muzzley::HTTPStatus _in) {
	this->__status = _in;
}

void muzzley::HTTPRepT::stringify(ostream& _out) {
	string _ret;
	this->stringify(_ret);
	_out << _ret << flush;
}

void muzzley::HTTPRepT::stringify(string& _out) {
	_out.insert(_out.length(), "HTTP/1.1 "),
	_out.insert(_out.length(),  muzzley::status_names[this->__status]);
	_out.insert(_out.length(), CRLF);
	for (auto i : this->__headers) {
		_out.insert(_out.length(), i.first);
		_out.insert(_out.length(), ": ");
		_out.insert(_out.length(), i.second);
		_out.insert(_out.length(), CRLF);
	}
	_out.insert(_out.length(), CRLF);
	_out.insert(_out.length(), this->__body);
}

muzzley::HTTPRep::HTTPRep()  : shared_ptr<HTTPRepT>(make_shared<HTTPRepT>()) {
}

muzzley::HTTPRep::HTTPRep(HTTPRepT* _target) : shared_ptr<HTTPRepT>(_target) {
}

muzzley::HTTPRep::~HTTPRep(){
}

void muzzley::HTTPRep::parse(istream& _in){
	muzzley::HTTPParser _p;
	_p.switchRoots(* this);
	_p.switchStreams(_in);
	_p.parse();
}

