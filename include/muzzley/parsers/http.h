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

#pragma once

#include <muzzley/config.h>
#ifdef HAVE_HTTP_CLIENT

#include <string>
#include <muzzley/http/HTTPObj.h>
#include <muzzley/parsers/HTTPParser.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace muzzley {

	muzzley::HTTPReq& fromhttpfile(ifstream& _in, muzzley::HTTPReq& _out) ;
	muzzley::HTTPRep& fromhttpfile(ifstream& _in, muzzley::HTTPRep& _out) ;
	muzzley::HTTPReq& fromhttpstream(istream& _in, muzzley::HTTPReq& _out) ;
	muzzley::HTTPRep& fromhttpstream(istream& _in, muzzley::HTTPRep& _out) ;
	muzzley::HTTPReq& fromhttpstr(string& _in, muzzley::HTTPReq& _out) ;
	muzzley::HTTPRep& fromhttpstr(string& _in, muzzley::HTTPRep& _out) ;

	void tostr(string& _out, HTTPReq& _in) ;
	void tostr(string& _out, HTTPRep& _in) ;
	void tostr(ostream& _out, HTTPReq& _in) ;
	void tostr(ostream& _out, HTTPRep& _in) ;
}

#endif