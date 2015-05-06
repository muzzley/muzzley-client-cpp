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


#pragma once

#include <exception>
#include <string>
#include <muzzley/base/assert.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace muzzley {

	/**
	 * \brief Exception to be thrown by *assertz* macro definition. The *assertz* macro should be used to create instances of this class. 
	 *
	 * This class allows the access to the line number and file name where the *assertz* failed and the related backtrace invocation heap.
	 */
	class AssertionException : public std::exception {
		private:
			string __what;
			int __http_code;
			int __code;
			string __description;
			int __line;
			string __file;
			string __backtrace;

		public:
			/**
			 * \brief Exception to be 
			 *
			 * @param _what           [description]
			 * @param _http_code      [description]
			 * @param _code           [description]
			 * @param _desc           [description]
			 * @param _line           [description]
			 * @param _file           [description]
			 * @param _backtrace      [description]
			 * @param _backtrace_size [description]
			 */
			AssertionException(string _what,  int _http_code, int _code, string _desc, int _line, string _file, char** _backtrace, size_t _backtrace_size);
			/**
			 * 
			 */
			virtual ~AssertionException() throw();

			/**
			 * \brief 
			 *
			 * @return [description]
			 */
			virtual const char* what() const throw();
			/**
			 * \brief 
			 *
			 * @return [description]
			 */
			virtual const char* description();
			/**
			 * \brief 
			 *
			 * @return [description]
			 */
			virtual const char* backtrace();
			/**
			 * \brief 
			 *
			 * @return [description]
			 */
			int code();
			/**
			 * \brief 
			 *
			 * @return [description]
			 */
			int status();
	};

}
