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

#define JSON(z) ((muzzley::JSONObj()) << z)
#define JSON_ARRAY(z) ((muzzley::JSONArr()) << z)
#define JSON_NIL muzzley::undefined;

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <sys/time.h>
#include <muzzley/config.h>
#include <muzzley/base/assert.h>
#include <muzzley/text/convert.h>
#include <muzzley/text/manip.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif


namespace muzzley {

	/**
	 * \brief Type definition for representing a millisecond based timestamp.
	 */
	typedef unsigned long long timestamp_t;

	class JSONElementT;
	class JSONObj;
	class JSONArr;

	class pretty : public std::string {
	public: 
		inline pretty(std::string _rhs) : std::string(_rhs) {
		};
		inline pretty(const char * _rhs) : std::string(_rhs) {
		};
		template <typename T>
		inline pretty(T _rhs) : std::string() {
			_rhs->prettify(* this);
		};

		friend ostream& operator<<(ostream& _out, muzzley::pretty& _in) {
			_out << string(_in.data());
			return _out;
		};				
	};

	/**
	 * \brief Smart shared pointer to a muzzley::JSONElementT object.
	 */
	class JSONPtr : public shared_ptr<JSONElementT> {
	public:
		/**
		 * \brief Creates a new JSONPtr instance, pointing to a *null* object.
		 */
		JSONPtr();
		/**
		 * \brief Creates a new JSONPtr instance, pointing to the *target* object.
		 */
		JSONPtr(JSONElementT* _target);
		/**
		 * \brief Destroys the current JSONPtr instance. It will only free the pointed object if there are no more *shared_ptr* objects pointing to it.
		 */
		virtual ~JSONPtr();

		/**
		 * \brief Read-access method for retrieving the value pointed by *this* instance.
		 *
		 * @return the value pointed by *this* instance
		 */
		JSONElementT& value();
		void parse(istream& _in);

		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		template <typename T>
		bool operator==(T _rhs);
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		template <typename T>
		bool operator!=(T _rhs);
		/**
		 * \brief Operator '<<' override form injecting values into *this* instance object. This is a convenience wrapper method for the muzzley::JSONObjT or muzzley::JSONArrT **push** methods.
		 * 
		 * According to *this* object type, behaviors differ:
		 *
		 * to add attributes to your object:
		 * 
		 *     muzzley::JSONObj _o;
		 *     _o << "name" << "Mr Muzzley";
		 *     _o << "serial" << 123;
		 *     _o << "sorting_field" << "name";
		 * 
		 * or
		 * 
		 *     muzzley::JSONObj _o;
		 *     _o << 
		 *       "name" << "Mr Muzzley" <<
		 *       "serial" << 123 <<
		 *       "sorting_field" << "name";
		 *     // this one is more JSON like
		 *
		 * (when *this* object is a muzzley::JSONObj and *T* is std::string, it will either be injected as an attribute name or as an attribute value, depending on whether or not you've already injected an attribute name)
		 * 
		 * to add a JSON array, use the *muzzley::JSONArr* class:
		 * 
		 *     muzzley::JSONArr _a;
		 *     _a << 123 << 345 << 67 << 78;
		 * 
		 *     muzzley::JSONArr _b;
		 *     _b << "lions" << 345 << "horses" << 78;
		 * 
		 *     muzzley::JSONObj _o;
		 *     _o << 
		 *       "name" << "Mr Muzzley" <<
		 *       "serial" << 123 <<
		 *       "sorting_field" << "name" <<
		 *       "numbers" << _a <<
		 *       "animal_numbers" << _b;
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 */
		template <typename T>
		JSONPtr& operator<<(T _in);
		/**
		 * \brief Operator '>>' override for removing attributes or array elements from *this* object instance. This is a convenience wrapper method for the muzzley::JSONObjT or muzzley::JSONArrT **pop** methods.
		 *
		 * Allowed types for *T* are: std::string, const char*, int, size_t,.
		 */
		template <typename T>
		JSONPtr& operator>>(T _in);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* object instance. This is a convenience wrapper method for the muzzley::JSONObjT or muzzley::JSONArrT '[]' operators.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_obj["some_array"][2]["first_value"] ...
		 * 
		 * Allowed types for *T* are: std::string, const char*, size_t.
		 *
		 * @return            the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		template <typename T>
		JSONPtr& operator[](T _idx);

		/**
		 * \brief Casting operator for the std::string class. **All** JSON types are castable to an std::string.
		 *
		 * @return the std::textual representation of *this* instance JSON typed object
		 */
		operator string();

		/**
		 * \brief Casting operator to a pretty printed std::string class. **All** JSON types are castable to an std::string.
		 *
		 * @return the pretty printed textual representation of *this* instance JSON typed object
		 */
		operator muzzley::pretty();

		/**
		 * \brief Casting operator for the *bool* basic type. **All** JSON types as castable to a *bool* value: 
		 * 
		 * - anything equals to a numerical zero or is empty, is castable to **false**
		 * - anything instantiated and different from a numerical zero is castable to **true**.
		 *
		 * @return the *bool* representation of *this* instance JSON typed object
		 */
		operator bool();
		/**
		 * \brief Casting operator for the *int* basic type. **All** JSON types as castable to a *int* value: 
		 * 
		 * - anything numerical is truncated to *int*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as an *int*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *int*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *int*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *int*
		 *
		 * @return the *int* representation of *this* instance JSON typed object
		 */
		operator int();
		/**
		 * \brief Casting operator for the *long* basic type. **All** JSON types as castable to a *long* value: 
		 * 
		 * - anything numerical is truncated to *long*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as a *long*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *long*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *long*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *long*
		 *
		 * @return the *long* representation of *this* instance JSON typed object
		 */
		operator long();
		/**
		 * \brief Casting operator for the *long long* basic type. **All** JSON types as castable to a *long long* value: 
		 * 
		 * - anything numerical is truncated to *long long*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as a *long long*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *long long*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *long long*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *long long*
		 *
		 * @return the *long long* representation of *this* instance JSON typed object
		 */
		operator long long();
#ifdef __LP64__
		/**
		 * \brief Casting operator for the *unsigned int* basic type. **All** JSON types as castable to a *unsigned int* value: 
		 * 
		 * - anything numerical is truncated to *unsigned int*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as an *unsigned int*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *unsigned int*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *unsigned int*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *unsigned int*
		 *
		 * @return the *unsigned int* representation of *this* instance JSON typed object
		 */
		operator unsigned int();
#endif
		/**
		 * \brief Casting operator for the *size_t* basic type. **All** JSON types as castable to a *size_t* value: 
		 * 
		 * - anything numerical is truncated to *size_t*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as a *size_t*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *size_t*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *size_t*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *size_t*
		 *
		 * @return the *size_t* representation of *this* instance JSON typed object
		 */
		operator size_t();
		/**
		 * \brief Casting operator for the *double* basic type. **All** JSON types as castable to a *double* value: 
		 * 
		 * - anything numerical is truncated to *double*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as a *double*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *double*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *double*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *double*
		 *
		 * @return the *double* representation of *this* instance JSON typed object
		 */
		operator double();
		/**
		 * \brief Casting operator for the *muzzley::timestamp_t* basic type. **All** JSON types as castable to a *muzzley::timestamp_t* value: 
		 * 
		 * - anything numerical is truncated to *muzzley::timestamp_t*
		 * - if *this* object instance is a JSON string that an attempt is made to parse the string value as a *muzzley::timestamp_t*, zero is returned if unsuccessful
		 * - if *this* object instance is a JSON object, the number of attributes is returned, truncated to *muzzley::timestamp_t*
		 * - if *this* object instance is a JSON array, the number of elements is returned, truncated to *muzzley::timestamp_t*
		 * - if *this* object instance is a JSON date, the number of milliseconds since *epoch* is returned, truncated to *muzzley::timestamp_t*
		 *
		 * @return the *muzzley::timestamp_t* representation of *this* instance JSON typed object
		 */
		operator timestamp_t();
		/**
		 * \brief Casting operator for *muzzley::JSONObj* class. If *this* instance object is not of type muzzley::JSONType::JSObject or muzzley::JSONType::JSNil, a muzzley::AssertionException is thrown.
		 *
		 * @return the *muzzley::JSONObj* representation of *this* instance JSON typed object
		 */
		operator JSONObj();
		/**
		 * \brief Casting operator for *muzzley::JSONArr* class. If *this* instance object is not of type muzzley::JSONType::JSArray or muzzley::JSONType::JSNil, a muzzley::AssertionException is thrown.
		 *
		 * @return the *muzzley::JSONArr* representation of *this* instance JSON typed object
		 */
		operator JSONArr();
		/**
		 * \brief Casting operator for *muzzley::JSONObj* class. If *this* instance object is not of type muzzley::JSONType::JSObject or muzzley::JSONType::JSNil, a muzzley::AssertionException is thrown.
		 *
		 * @return the *muzzley::JSONObj* representation of *this* instance JSON typed object
		 */
		operator JSONObj&();
		/**
		 * \brief Casting operator for *muzzley::JSONArr* class. If *this* instance object is not of type muzzley::JSONType::JSArray or muzzley::JSONType::JSNil, a muzzley::AssertionException is thrown.
		 *
		 * @return the *muzzley::JSONArr* representation of *this* instance JSON typed object
		 */
		operator JSONArr&();

		/**
		 * \brief Friendly '>>' std::istream operator override that parses the textual representation available on an std::istream object into a of a muzzley::JSONPtr object.
		 */
		friend istream& operator>>(istream& _in, JSONPtr& _out) {
			_out.parse(_in);
			return _in;
		};

	};

	typedef JSONPtr JSONElement;

	/**
	 * \brief Convenience global variable that represents the *undefined* JSON type, to be used in comparisons and default return values.
	 *
	 * Example:
	 * 
	 *     if (my_json_object["some_attribute"] == muzzley::undefined) {
	 *         ...
	 *     } 
	 */
	extern JSONPtr undefined;
	extern JSONPtr nilptr;

	template <typename T>	
	muzzley::JSONElementT * make_element(T& _e) {
		return new muzzley::JSONElementT(_e);
	}

	template <typename T>	
	muzzley::JSONPtr make_ptr(T& _e) {
		return muzzley::JSONPtr(new muzzley::JSONElementT(_e));
	}

	/**
	 * \brief Class that represents the *object* JSON type. It inherits from the std::map class and is composed of std::string and muzzley::JSONPtr key-value pairs.
	 */
	class JSONObjT : public map< string, JSONPtr > {
	public: 
		/**
		 * \brief Creates a new JSONObjT instance.
		 */		
		JSONObjT();
		/**
		 * \brief Destroys the current JSONObjT instance, freeing all allocated memory. It will free the objects pointed by each muzzley::JSONPtr smart pointer only if there aren't any more std::shared_ptr pointing to it.
		 */
		virtual ~JSONObjT();

		/**
		 * \brief Retrieves the textual representation of *this* JSON object instance. The textual representation has no empty characters, that is, no spaces, tabs or new lines.
		 *
		 * @param _out the textual representation for the JSON object
		 */
		virtual void stringify(string& _out);
		/**
		 * \brief Outputs to the std::ostring *out* the textual representation of *this* JSON object instance. The textual representation has no empty characters, that is, no spaces, tabs or new lines.
		 *
		 * @param _out the std::ostream to output the JSON object representation to
		 */
		virtual void stringify(ostream& _out);

		/**
		 * \brief Retrieves a human readable textual representation of *this* JSON object instance. The textual representation is multi-line indented.
		 *
		 * @param _out    the textual representation for the JSON object
		 * @param _n_tabs the initial number of tabs to indent
		 */
		virtual void prettify(string& _out, uint _n_tabs = 0);
		/**
		 * \brief Outputs to the std::ostring *out* a human readable textual representation of *this* JSON object instance. The textual representation is multi-line indented.
		 *
		 * @param _out    the std::ostream to output the JSON object representation to
		 * @param _n_tabs the initial number of tabs to indent
		 */
		virtual void prettify(ostream& _out, uint _n_tabs = 0);

		/**
		 * \brief Write-access method that inserts an std::string into *this* object map. 
		 * 
		 * If there isn't any std::string in context to be used as name/key the *name* string will be used as one. If there is an std::string in context, then the *name* string is used as value.
		 *
		 * @param _name the attribute name or attribute value
		 */
		virtual void push(string _name);
		/**
		 * \brief Write-access method that inserts a muzzley::JSONElementT object into *this* object map. 
		 *
		 * @param _value [description]
		 */
		virtual void push(JSONElementT& _value);
		/**
		 * \brief Write-access method that inserts a muzzley::JSONElementT object into *this* object map. 
		 *
		 * @param _value [description]
		 */
		virtual void push(JSONElementT* _value);

		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(int _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(size_t _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(const char* _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(string _idx);

		/**
		 * \brief Read-access method for retrieving a child element represented by the *path* object path.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_object["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array/0/first_field/name", "/");
		 *
		 * @param  _path      the object path to search for
		 * @param  _separator the object path separator
		 *
		 * @return            the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		muzzley::JSONPtr getPath(std::string _path, std::string _separator = ".");

		/**
		 * \brief Write-access method for adding a child element represented by the *path* object path and with *value* value.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_object["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array/0/first_field/name", "/");
		 *
		 * @param _path      the object path to search for
		 * @param _value     the value to be assigned to the child element
		 * @param _separator the object path separator
		 */
		void setPath(std::string _path, muzzley::JSONPtr _value, std::string _separator = ".");

		/**
		 * \brief Write-access method for removing a child element represented by the *path* object path.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_object["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array/0/first_field/name", "/");
		 *
		 * @param  _path      the object path to search for
		 * @param  _separator the object path separator
		 */
		void delPath(std::string _path, std::string _separator = ".");

		/**
		 * @brief Creates a full copy of the JSON representation stored in *this* *muzzley::JSONObjT*
		 * @return a pointer to the copy of the underlying JSON representation
		 */
		JSONPtr clone();

		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		bool operator==(JSONObjT& _in);
		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		bool operator==(JSONObj& _in);
		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		template <typename T>
		bool operator==(T _in) {
			return false;
		};
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		bool operator!=(JSONObjT& _in);
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		bool operator!=(JSONObj& _in);
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		template <typename T>
		bool operator!=(T _in) {
			return true;
		};

		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* object instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_obj["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](int _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* object instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_obj["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](size_t _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* object instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_obj["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](const char* _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* object instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_obj["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](string _idx);

		/**
		 * \brief Friendly '<<' std::ostream operator override that outputs the textual representation of *this* object instance into the *out* stream.
		 */
		friend ostream& operator<<(ostream& _out, JSONObjT& _in) {
			_in.stringify(_out);
			return _out;
		};

	private:
		string __name;
	};

	/**
	 * \brief Class that represents the *array* JSON type. It inherits from the std::vector class and is composed of muzzley::JSONPtr elements.
	 */
	class JSONArrT : public vector<JSONPtr > {
	public: 
		/**
		 * \brief Creates a new JSONArrT instance.
		 */		
		JSONArrT();
		/**
		 * \brief Destroys the current JSONArrT instance, freeing all allocated memory. It will free the objects pointed by each muzzley::JSONPtr smart pointer only if there aren't any more std::shared_ptr pointing to it.
		 */
		virtual ~JSONArrT();

		/**
		 * \brief Retrieves the textual representation of *this* JSON array instance. The textual representation has no empty characters, that is, no spaces, tabs or new lines.
		 *
		 * @param _out the textual representation for the JSON array
		 */
		virtual void stringify(string& _out);
		/**
		 * \brief Outputs to the std::ostring *out* the textual representation of *this* JSON array instance. The textual representation has no empty characters, that is, no spaces, tabs or new lines.
		 *
		 * @param _out the std::ostream to output the JSON array representation to
		 */
		virtual void stringify(ostream& _out);

		/**
		 * \brief Retrieves a human readable textual representation of *this* JSON array instance. The textual representation is multi-line indented.
		 *
		 * @param _out    the textual representation for the JSON array
		 * @param _n_tabs the initial number of tabs to indent
		 */
		virtual void prettify(string& _out, uint _n_tabs = 0);
		/**
		 * \brief Outputs to the std::ostring *out* a human readable textual representation of *this* JSON array instance. The textual representation is multi-line indented.
		 *
		 * @param _out    the std::ostream to output the JSON array representation to
		 * @param _n_tabs the initial number of tabs to indent
		 */
		virtual void prettify(ostream& _out, uint _n_tabs = 0);

		/**
		 * \brief Write-access method that inserts a muzzley::JSONElementT object into *this* array vector. 
		 *
		 * @param _value [description]
		 */
		virtual void push(JSONElementT& _value);
		/**
		 * \brief Write-access method that inserts a muzzley::JSONElementT object into *this* array vector. 
		 *
		 * @param _value [description]
		 */
		virtual void push(JSONElementT* _value);

		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(int _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(size_t _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(const char* _idx);
		/**
		 * \brief Write-access method that removes the JSON element identified by *idx*.
		 *
		 * @param _idx the identification of the element to remove
		 */
		virtual void pop(string _idx);

		/**
		 * \brief Read-access method for retrieving a child element represented by the *path* object path.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_array[1]["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_array->getPath("1.some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_array->getPath("1/some_array/0/first_field/name", "/");
		 *
		 * @param  _path      the object path to search for
		 * @param  _separator the object path separator
		 *
		 * @return            the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		muzzley::JSONPtr getPath(std::string _path, std::string _separator = ".");

		/**
		 * \brief Write-access method for adding a child element represented by the *path* object path and with *value* value.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_object["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array/0/first_field/name", "/");
		 *
		 * @param _path      the object path to search for
		 * @param _value     the value to be assigned to the child element
		 * @param _separator the object path separator
		 */
		void setPath(std::string _path, muzzley::JSONPtr _value, std::string _separator = ".");

		/**
		 * \brief Write-access method for removing a child element represented by the *path* object path.
		 *
		 * An object path is sequence of child object identifiers, separated by a given character. For instance, the following code
		 *
		 *     muzzley::JSONPtr child = my_json_object["some_array"][0]["first_field"]["name"];
		 *
		 * is analogue to
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array.0.first_field.name");
		 *
		 * or 
		 *
		 *     muzzley::JSONPtr child = my_json_object->getPath("some_array/0/first_field/name", "/");
		 *
		 * @param  _path      the object path to search for
		 * @param  _separator the object path separator
		 */
		void delPath(std::string _path, std::string _separator = ".");

		/**
		 * @brief Creates a full copy of the JSON representation stored in *this* *muzzley::JSONArrT*
		 * @return a pointer to the copy of the underlying JSON representation
		 */
		JSONPtr clone();

		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		bool operator==(JSONArrT& _in);
		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		bool operator==(JSONArr& _in);
		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		template <typename T>
		bool operator==(T _in) {
			return false;
		};
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		bool operator!=(JSONArrT& _in);
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		bool operator!=(JSONArr& _in);
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		template <typename T>
		bool operator!=(T _in) {
			return true;
		};

		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* array instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_arr[1]["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](int _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* array instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_arr[1]["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](size_t _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* array instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_arr[1]["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](const char* _idx);
		/**
		 * \brief Operator '[]' override for accessing attributes or array elements of *this* array instance.
		 *
		 * Returns the attribute or array element identified by *idx*. It allows chaining, for instance:
		 *
		 *     my_json_arr[1]["some_array"][2]["first_value"] ...
		 * 
		 * @param  _idx the child object identifier
		 *
		 * @return  the pointer to the child object if it exists or muzzley::undefined otherwise
		 */
		JSONPtr& operator[](string _idx);

		/**
		 * \brief Friendly '<<' std::ostream operator override that outputs the textual representation of *this* array instance into the *out* stream.
		 */
		friend ostream& operator<<(ostream& _out, JSONArrT& _in) {
			_in.stringify(_out);
			return _out;
		};

	};	

	/**
	 * \brief Smart shared pointer to a muzzley::JSONObjT object.
	 */
	class JSONObj : public shared_ptr<JSONObjT> {
	public:
		/**
		 * \brief Creates a new JSONObj instance, pointing to a *null* object.
		 */
		JSONObj();
		/**
		 * \brief Creates a new JSONObj instance copying the target reference from *rhs*.
		 *
		 * @param _rhs the smart pointer to copy the target from
		 */
		JSONObj(JSONObj& _rhs);
		/**
		 * \brief Creates a new JSONObj instance, pointing to the *target* object.
		 */
		JSONObj(JSONObjT* _target);
		/**
		 * \brief Destroys the current JSONObj instance. It will only free the pointed object if there are no more *shared_ptr* objects pointing to it.
		 */
		virtual ~JSONObj();

		/**
		 * \brief Retrieves an iterator pointing to the beginning of *this* object attribute list. 
		 *
		 * @return the iterator pointing to the beginning of *this* object attribute list
		 */
		JSONObjT::iterator begin();
		/**
		 * \brief Retrieves an iterator pointing to the end of *this* object attribute list. 
		 *
		 * @return the iterator pointing to the end of *this* object attribute list
		 */
		JSONObjT::iterator end();

		/**
		 * \brief Cast operation for the std::string class. This is a convenience wrapper operator for muzzley::JSONObjT::stringify method.
		 *
		 * @return the textual representation of *this* object instance
		 */
		operator string();

		/**
		 * \brief Casting operator to a pretty printed std::string class. **All** JSON types are castable to an std::string.
		 *
		 * @return the pretty printed textual representation of *this* instance JSON typed object
		 */
		operator muzzley::pretty();

		/**
		 * \brief Operator '==' override for comparing *this* instance with other JSON typed argument. Type conversion between JSON type is attempted in order to determine the objects equality.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are equal, **false** otherwise
		 */
		template <typename T>
		bool operator==(T _rhs) {
			return *(this->get()) == _rhs;
		};
		/**
		 * \brief Operator '!=' override for comparing *this* instance with other JSON typed argument.
		 *
		 * Allowed types for *T* are: muzzley::JSONElementT, muzzley::JSONPtr, muzzley::JSONObj, muzzley::JSONArr, std::string, const char*, long long, double, bool, muzzley::timestamp_t, int, size_t.
		 *
		 * @return **true** if the objects are different, **false** otherwise
		 */
		template <typename T>
		bool operator!=(T _rhs) {
			return *(this->get()) != _rhs;
		};
		JSONObj& operator<<(string _in);
		JSONObj& operator<<(const char* _in);
		template <typename T>
		JSONObj& operator<<(T _in) {
			(* this)->push(new JSONElementT(_in));
			return * this;
		};
		template <typename T>
		JSONObj& operator>>(T _in) {
			(* this)->pop(_in);
			return * this;
		};
		template <typename T>
		JSONPtr& operator[](T _idx) {
			return (*(this->get()))[_idx];
		};

		friend ostream& operator<<(ostream& _out, JSONObj& _in) {
			_in->stringify(_out);
			return _out;
		};		
	};

	class JSONArr : public shared_ptr<JSONArrT> {
	public:
		JSONArr();
		JSONArr(JSONArr& _rhs);
		JSONArr(JSONArrT* _target);
		virtual ~JSONArr();

		JSONArrT::iterator begin();
		JSONArrT::iterator end();

		operator string();

		/**
		 * \brief Casting operator to a pretty printed std::string class. **All** JSON types are castable to an std::string.
		 *
		 * @return the pretty printed textual representation of *this* instance JSON typed object
		 */
		operator muzzley::pretty();

		template <typename T>
		bool operator==(T _rhs) {
			return *(this->get()) == _rhs;
		};
		template <typename T>
		bool operator!=(T _rhs) {
			return *(this->get()) != _rhs;
		};
		template <typename T>
		JSONArr& operator<<(T _in) {
			(* this)->push(new JSONElementT(_in));
			return * this;
		};
		template <typename T>
		JSONArr& operator>>(T _in) {
			(* this)->pop(_in);
			return * this;
		};
		template <typename T>
		JSONPtr& operator[](T _idx) {
			return (*(this->get()))[_idx];
		};

		friend ostream& operator<<(ostream& _out, JSONArr& _in) {
			_in->stringify(_out);
			return _out;
		};		
	};

	typedef shared_ptr<string> JSONStr;

	typedef struct JSONStruct {
		JSONStruct() : __type(JSNil) { };
		~JSONStruct() { 
			switch(__type) {
				case muzzley::JSObject : {
					__object.~JSONObj();
					break;
				} case muzzley::JSArray : {
					__array.~JSONArr();
					break;
				} case muzzley::JSString : {
					__string.~JSONStr();
					break;
				} default : {
					break;
				}
			} 
		};

		JSONStruct(JSONStruct const&) = delete;
		JSONStruct& operator=(JSONStruct const&) = delete;

		JSONStruct(JSONStruct&&) = delete;
		JSONStruct& operator=(JSONStruct&&) = delete;

		JSONType __type;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
		union {
#endif
			JSONObj __object;
			JSONArr __array;
			JSONStr __string;
			long long __integer;
			double __double;
			bool __boolean;
			void* __nil;
			muzzley::timestamp_t __date;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
		};
#endif
	} JSONUnion;

	class JSONElementT {
	public:
		JSONElementT();
		JSONElementT(JSONElementT& _element);
		JSONElementT(JSONPtr& _value);
		JSONElementT(JSONObj& _value);
		JSONElementT(JSONArr& _value);
		JSONElementT(string _value);
		JSONElementT(const char* _value);
		JSONElementT(long long _value);
		JSONElementT(double _value);
		JSONElementT(bool _value);
		JSONElementT(muzzley::timestamp_t _value);
		JSONElementT(int _value);
		JSONElementT(size_t _value);
#ifdef __LP64__
		JSONElementT(unsigned int _value);
#endif
		virtual ~JSONElementT();

		virtual JSONType type();
		virtual string demangle();
		virtual void type(JSONType _in);
		virtual JSONUnion& value();
		virtual bool ok();
		virtual bool empty();
		virtual bool nil();

		virtual void assign(JSONElementT& _rhs);

		JSONElementT* parent();
		void parent(JSONElementT* _parent);

		/**
		 * @brief Creates a full copy of the JSON representation pointed by this *muzzley::JSONPtr*
		 * @return a pointer to the copy of the underlying JSON representation
		 */
		JSONPtr clone();

		virtual JSONObj& obj();
		virtual JSONArr& arr();
		string str();
		long long intr();
		double dbl();
		bool bln();
		muzzley::timestamp_t date();
		double number();

		JSONElementT& operator<<(const char* _in);
		JSONElementT& operator<<(string _in);
		JSONElementT& operator<<(JSONElementT*);
		template <typename T>
		JSONElementT& operator<<(T _in){
			assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
			switch(this->__target.__type) {
				case muzzley::JSObject : {
					this->__target.__object->push(new JSONElementT(_in));
					break;
				} case muzzley::JSArray : {
					this->__target.__array->push(new JSONElementT(_in));
					break;
				} default : {
					JSONElementT _e(_in);
					this->assign(_e);
					assertz(this->__target.__type >= 0, "the type must be a valid value", 0, 0);
					break;
				}
			}
			return * this;
		};
		template <typename T>
		JSONElementT& operator>>(T _in){
			switch(this->__target.__type) {
				case muzzley::JSObject : {
					this->__target.__object >> _in;
					break;
				}
				case muzzley::JSArray : {
					this->__target.__array >> _in;
					break;
				}
				default : {
					break;
				}
			}
			return * this;
		};
		template <typename T>
		JSONPtr& operator[](T _idx) {
			if (this->__target.__type == muzzley::JSObject) {
				return this->__target.__object [_idx];
			}
			else if (this->__target.__type == muzzley::JSArray) {
				return this->__target.__array [_idx];
			}
			return muzzley::undefined;
		};
		bool operator==(JSONElementT& _in);
		bool operator==(JSONPtr& _rhs);
		template <typename T>
		bool operator==(T _in) {
			JSONElementT _rhs(_in);
			return (* this) == _rhs;
		};
		bool operator!=(JSONElementT& _in);
		bool operator!=(JSONPtr& _rhs);
		template <typename T>
		bool operator!=(T _in) {
			if (_in == nullptr) {
				return this->__target.__type == muzzley::JSNil; 
			}
			JSONElementT _rhs(_in);
			return (* this) == _rhs;
		};

		friend ostream& operator<<(ostream& _out, JSONElementT _in) {
			_in.stringify(_out);
			return _out;
		};

		muzzley::JSONPtr getPath(std::string _path, std::string _separator = ".");
		void setPath(std::string _path, muzzley::JSONPtr _value, std::string _separator = ".");
		void delPath(std::string _path, std::string _separator = ".");

		virtual void stringify(string& _out);
		virtual void stringify(ostream& _out);
		virtual string stringify();

		virtual void prettify(string& _out, uint _n_tabs = 0);
		virtual void prettify(ostream& _out, uint _n_tabs = 0);

	private:
		JSONUnion __target;
		JSONElementT* __parent;

		void init();
	};

}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <typename T>
bool muzzley::JSONPtr::operator==(T _rhs) {
	return *(this->get()) == _rhs;
};
template <typename T>
bool muzzley::JSONPtr::operator!=(T _rhs){
	return *(this->get()) != _rhs;
};
template <typename T>
muzzley::JSONPtr& muzzley::JSONPtr::operator<<(T _in) {
	*(this->get()) << _in;
	return * this;
};
template <typename T>
muzzley::JSONPtr& muzzley::JSONPtr::operator>>(T _in) {
	*(this->get()) >> _in;
	return * this;
};
template <typename T>
muzzley::JSONPtr& muzzley::JSONPtr::operator[](T _idx) {
	return (*(this->get()))[_idx];
};
#endif