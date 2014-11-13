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


#include <muzzley/stream/WebSocket.h>

bool muzzley::ws::read(muzzley::socketstream& _s, string& _out, int* _op_code) {
	unsigned char _hdr;
	_s >> noskipws >> _hdr;

	bool _fin = _hdr & 0x80;
	*_op_code = _hdr & 0x0F;
	_s >> noskipws >> _hdr;
	bool _mask = _hdr & 0x80;

	int _len = _hdr & 0x7F;
	if (_len == 126) {
		_s >> noskipws >> _hdr;
		_len = (int) _hdr;
		_len <<= 8;
		_s >> noskipws >> _hdr;
		_len += (int) _hdr;
	}
	else if (_len == 127) {
		_s >> noskipws >> _hdr;
		_len = (int) _hdr;
		for (int _i = 0; _i < 7; _i++) {
			_len <<= 8;
			_s >> noskipws >> _hdr;
			_len += (int) _hdr;
		}
	}

	if (_mask) {
		for (int _i = 0; _i < 4; _i++) {
			_s >> noskipws >> _hdr;
		}
	}

	for (int _i = 0; _i != _len; _i++) {
		_s >> noskipws >> _hdr;
		_out.push_back((char) _hdr);
	}

	return _fin;
}

bool muzzley::ws::write(muzzley::socketstream& _s, string _in){
	int _len = _in.length();

	_s << (unsigned char) 0x81;
	if (_len > 125) {
		_s << (unsigned char) 0xFE;
		_s << ((unsigned char) (_len >> 8));
		_s << ((unsigned char) (_len & 0xFF));
	}
	else {
		_s << (unsigned char) (0x80 | ((unsigned char) _len));
	}
	for (int _i = 0; _i != 4; _i++) {
		_s << (unsigned char) 0x00;
	}

	_s << _in << flush;

	return true;

}
