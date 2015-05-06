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

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace muzzley {

	/**
	 * \brief Exception to be thrown when an attempt to set a JSON object value without setting the attribute name
	 */
	class NoAttributeNameException: public std::exception {
		private:
			string __what;

		public:
			/**
			 * \brief Creates a new NoAttributeNameException instance.
			 *
			 * @param _what brief but comprehensive description of the exception
			 */
			NoAttributeNameException(string _what);
			/**
			 * \brief Destroys the current NoAttributeNameException instance, freeing all allocated memory.
			 */
			virtual ~NoAttributeNameException() throw();

			/**
			 * \brief Read-access method for the brief description.
			 *
			 * @return the brief description
			 */
			virtual const char* what() const throw();
	};

}
