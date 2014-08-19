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

#include <resource/Client.h>
#include <base/smart_ptr.h>
#include <base/assert.h>
#include <parsers/json.h>
#include <pthread.h>
#include <iterator>
#include <bitset>
#include <log/log.h>

muzzley::Client::Client() :
	__serial(1) {
	pthread_key_create(&muzzley::__memory_key, NULL);
	this->__mtx = new pthread_mutex_t();
	this->__thr = new pthread_t();
	pthread_mutexattr_init(&this->__attr);
	pthread_mutex_init(this->__mtx, &this->__attr);

	this->__is_loop_assynchronous = false;
	this->__op_code = -1;

	this->__is_app_loggedin = false;
	this->__is_user_loggedin = false;
	this->__is_initiating_master = false;
	this->__has_handshake = false;

	this->__participant_id = -1;

	muzzley::log_fd = &cout;

	this->on(muzzley::Connect, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && !!_data["d"]["deviceId"]) {
			_client.__device_id.assign((string) _data["d"]["deviceId"]);
		}
		return true;
	});

	this->on(muzzley::AppLoggedIn, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && !!_data["d"]["sessionId"]) {
			_client.__session_id.assign((string) _data["d"]["sessionId"]);
			_client.__is_app_loggedin = true;
			_client.__is_user_loggedin = false;
			_client.createActivity(_client.__activity_id);
		}
		return true;
	});

	this->on(muzzley::UserLoggedIn, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && !!_data["d"]["sessionId"]) {
			_client.__session_id.assign((string) _data["d"]["sessionId"]);
			_client.__is_app_loggedin = false;
			_client.__is_user_loggedin = true;
			_client.joinActivity(_client.__activity_id);
		}
		return true;
	});

	this->on(muzzley::ActivityJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"]) {
			_client.__participant_id = (long) _data["d"]["participant"]["id"];
			_client.participantReady();
		}
		return true;
	});

	this->on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && !!_data["d"]["activityId"]) {
			_client.__activity_id.assign((string) _data["d"]["activityId"]);
			_client.__is_initiating_master = true;
		}
		return true;
	});

	this->on(muzzley::ActivityTerminated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		_client.__activity_id.assign("");
		_client.__is_initiating_master = false;
		if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
					"s" << true
				));
		}
		return true;
	});

	this->on(muzzley::ParticipantQuit, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["d"]["participantId"]) {
			pthread_mutex_lock(_client.__mtx);
			muzzley::ParticipantList::iterator _found = _client.__participants.find((long) _data["d"]["participantId"]);
			if (_found != _client.__participants.end()) {
				_client.__participants.erase(_found);
			}
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				_client.reply(_data, JSON(
						"s" << true
					));
			}
		}
		else if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
					"s" << false
				));
		}
		return true;
	});

	this->on(muzzley::ParticipantJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["h"]["pid"] && !!_data["d"]) {
			long _pid = (long) _data["h"]["pid"];

			pthread_mutex_lock(_client.__mtx);
			muzzley::ParticipantList::iterator _found = _client.__participants.find(_pid);
			if (_found != _client.__participants.end()) {
				muzzley::JSONObj _participant;
				muzzley::fromstr(_found->second, _participant);
				((muzzley::JSONObj) _data) << "d" << _participant;
			}
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				_client.reply(_data, JSON(
						"h" << JSON (
							"pid" << _pid
						) <<
						"s" << true
					));
			}
		}
		else {
			_client.reply(_data, JSON(
					"s" << false
				));
		}
		return true;
	});

	this->on(muzzley::__INTERNAL_ParticipantJoined__, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["d"]["participant"]["id"]) {
			if (((string) _data["d"]["participant"]["deviceId"]) == _client.__device_id) {
				_client.__participant_id = (long) _data["d"]["participant"]["id"];
			}
			pthread_mutex_lock(_client.__mtx);
			_client.__participants.insert(make_pair((long) _data["d"]["participant"]["id"], (string) _data["d"]));
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				_client.reply(_data, JSON(
						"s" << true
					));
			}
		}
		else if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
					"s" << false
				));
		}
		return true;
	});

	this->on(muzzley::ChangeWidget, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
					"s" << true
				));
		}
		return true;
	});

	this->on(muzzley::SetupComponent, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
					"s" << true
				));
		}
		return true;
	});

}

muzzley::Client::~Client() {
	pthread_mutexattr_destroy(&this->__attr);
	pthread_mutex_destroy(this->__mtx);
	delete this->__mtx;
	delete this->__thr;
	pthread_exit(NULL);
}

#if __cplusplus >= 201103L
void muzzley::Client::on(muzzley::EventType _type, muzzley::Handler _handler) {
	this->__handlers[_type].push_back(_handler);
}

void muzzley::Client::trigger(muzzley::EventType _type, muzzley::JSONObj& _data) {
	if (_type < 0 || _type > N_EVENT_TYPES) {
		return;
	}
	for (std::vector<muzzley::Handler>::iterator _i = this->__handlers[_type].begin(); _i != this->__handlers[_type].end(); _i++) {
		if (!(*_i)(_data, *this)) {
			return;
		}
	}
}
#endif

bool muzzley::Client::connect(string _host, uint16_t _port, string _path) {
	string _message("GET ");
	_message.insert(_message.length(), _path);
	_message.insert(_message.length(), " HTTP/1.1");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Upgrade: websocket");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Connection: Upgrade");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Host: ");
	_message.insert(_message.length(), _host);
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Origin: C++WSClient");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Sec-WebSocket-Version: 13");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Sec-WebSocket-Key: ");

	string _key;
	for (int i = 0; i != 16; i++) {
		_key.push_back(255 * random());
	}
	muzzley::base64_encode(_key);
	_message.insert(_message.length(), _key);
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), CRLF);

	this->__channel.open(_host, _port);
#ifdef MUZZLEY_DEBUG
	{
		string _log("connecting to ");
		_log.insert(_log.length(), _host);
		_log.insert(_log.length(), ":");
		muzzley::tostr(_log, _port);
		muzzley::log(_log, muzzley::sys);
	}
#endif
	this->__channel << _message << flush;

	string _reply;
	do {
		getline(this->__channel, _reply);
		muzzley::trim(_reply);
	}
	while (_reply != "");
#ifdef MUZZLEY_DEBUG
	{
		string _log("received WebSocket handshake");
		muzzley::log(_log, muzzley::sys);
	}
#endif

	return true;
}

void muzzley::Client::disconnect() {
	this->__channel << (uint8_t) 0x87;
	this->__channel << (uint8_t) 0x00;
	this->__channel << flush;
	this->__channel.close();
	this->__is_connected = false;
}

void muzzley::Client::reconnect() {
	if (this->isConnected()) {
		this->disconnect();
	}
	if (this->connect(MUZZLEY_ENDPOINT_ADDRESS, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		this->__is_connected = true;
		this->handshake(NULL);
	}
}

bool muzzley::Client::read() {
	unsigned char _hdr;
	this->__channel >> noskipws >> _hdr;

	//cout << "bit 1: "; muzzley::printbits(_hdr);

	bool _fin = _hdr & 0x80;
	this->__op_code = _hdr & 0x0F;
	this->__channel >> noskipws >> _hdr;
	bool _mask = _hdr & 0x80;

	//cout << "bit 2: "; muzzley::printbits(_hdr);

	int _len = _hdr & 0x7F;
	//cout << "len: " << _len << endl << flush;
	if (_len == 126) {
		this->__channel >> noskipws >> _hdr;
		_len = (int) _hdr;
		_len <<= 8;
		this->__channel >> noskipws >> _hdr;
		_len += (int) _hdr;
	}
	else if (_len == 127) {
		this->__channel >> noskipws >> _hdr;
		_len = (int) _hdr;
		for (int _i = 0; _i < 7; _i++) {
			_len <<= 8;
			this->__channel >> noskipws >> _hdr;
			_len += (int) _hdr;
		}
	}

	if (_mask) {
		for (int _i = 0; _i < 4; _i++) {
			this->__channel >> noskipws >> _hdr;
		}
	}

	for (int _i = 0; _i != _len; _i++) {
		//cout << "step: " << _i << endl << flush;
		this->__channel >> noskipws >> _hdr;
		this->__message.push_back((char) _hdr);
	}

	return _fin;
}

bool muzzley::Client::write(muzzley::JSONObj& _message, muzzley::Callback _callback) {
	if ((muzzley::MessageType) ((int) _message["h"]["t"]) == muzzley::RequestInitiatedByEndpoint && !!_message["h"]["cid"]) {
		string _action = (string) _message["a"];
		pthread_mutex_lock(this->__mtx);
		this->__queue.insert(make_pair(this->__serial, make_pair(_action, _callback)));
		pthread_mutex_unlock(this->__mtx);
		this->__serial++;
	}

	string _to_send;
	muzzley::tostr(_to_send, _message);
	int _len = _to_send.length();

	this->__channel << (unsigned char) 0x81;
	if (_len > 125) {
		this->__channel << (unsigned char) 0xFE;
		this->__channel << ((unsigned char) (_len >> 8));
		this->__channel << ((unsigned char) (_len & 0xFF));
	}
	else {
		this->__channel << (unsigned char) (0x80 | ((unsigned char) _len));
	}
	for (int _i = 0; _i != 4; _i++) {
		this->__channel << (unsigned char) 0x00;
	}

	this->__channel << _to_send << flush;

	return true;
}

bool muzzley::Client::reply(muzzley::JSONObj& _data_received, muzzley::JSONObj& _reply) {
	assertz(!!_reply["s"], "attribute 's' is mandatory, in the '_data_received' argument", 500, 108);
	assertz(!!_data_received["h"]["t"], "there is no message type in the '_data_received' argument", 500, 107);
	assertz(!!_data_received["h"]["cid"], "there is no message ide ('cid') in the '_data_received' argument", 500, 107);
	muzzley::MessageType _type = (muzzley::MessageType) ((int) _data_received["h"]["t"]);

	switch (_type) {
		case muzzley::RequestInitiatedByEndpoint: {
			if (!!_reply["h"]) {
				((muzzley::JSONObj) _reply["h"]) <<
					"t" << (int) muzzley::ReplyToEndpoint <<
					"cid" << (string) _data_received["h"]["cid"];
			}
			else {
				_reply <<
					"h" << JSON(
						"t" << (int) muzzley::ReplyToEndpoint <<
						"cid" << (string) _data_received["h"]["cid"]
						);
			}
			break;
		}
		case muzzley::RequestInitiatedMuzzleyCore: {
			if (!!_reply["h"]) {
				((muzzley::JSONObj) _reply["h"]) <<
					"t" << (int) muzzley::ReplyToEndpoint <<
					"cid" << (string) _data_received["h"]["cid"];
			}
			else {
				_reply <<
					"h" << JSON(
						"t" << (int) muzzley::ReplyToMuzzleyCore <<
						"cid" << (string) _data_received["h"]["cid"]
						);
			}
			break;
		}
		default:
			return false;
	}

	return this->write(_reply);
}

void muzzley::Client::connectApp(string _app_token, string _activity_id) {
	this->__activity_id.assign(_activity_id.data());
	this->__is_static_activity = _activity_id.length() != 0;
	if (this->connect(MUZZLEY_ENDPOINT_ADDRESS, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		this->__is_connected = true;
		if (this->__is_loop_assynchronous) {
			this->start();
			this->handshake([ _app_token ] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginApp(_app_token);
				return true;
			});
		}
		else {
			this->handshake([ _app_token ] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginApp(_app_token);
				return true;
			});
			this->run();
		}
	}
}

void muzzley::Client::connectUser(string _user_token, string _activity_id) {
	assertz(_activity_id.length() != 0, "when connecting a user, the '_activity_id' parameter must be instantiated with a non-empty string", 500, 100);
	this->__activity_id.assign(_activity_id.data());
	this->__is_static_activity = _activity_id.length() != 0;
	if (this->connect(MUZZLEY_ENDPOINT_ADDRESS, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		this->__is_connected = true;
		if (this->__is_loop_assynchronous) {
			this->start();
			this->handshake([ _user_token ] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginUser(_user_token);
				return true;
			});
		}
		else {
			this->handshake([ _user_token ] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginUser(_user_token);
				return true;
			});
			this->run();
		}
	}
}

void muzzley::Client::loginApp(string _app_token) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "loginApp" <<
		"d" << JSON(
			"token" << _app_token
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("login app:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::loginUser(string _user_token) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "loginUser" <<
		"d" << JSON(
			"token" << _user_token
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("login user:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::createActivity(string _activity_id) {
	assertz(this->__is_app_loggedin, "activity can only be created by Activity Master, you must be logged in as as app (use connectApp method).", 500, 101);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "create";


	if (_activity_id.length() != 0) {
		this->__activity_id.assign(_activity_id.data());
	}
	else {
		_activity_id.assign(this->__activity_id.data());
	}

	if (_activity_id.length() != 0) {
		_message <<
			"d" << JSON(
				"activityId" << _activity_id
				);
	}

#ifdef MUZZLEY_DEBUG
	{
		string _log("creating activity:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::joinActivity(string _activity_id) {
	assertz(_activity_id.length() != 0 || (_activity_id.length() == 0 && this->__activity_id.length() != 0), "an activityId must be provided, either in joinActivity, connectApp or connectUser method invocations", 500, 103);
	assertz(this->__is_user_loggedin, "joinActivity can only be invoked by a user", 500, 106);

	if (_activity_id.length() != 0) {
		this->__activity_id.assign(_activity_id.data());
	}
	else {
		_activity_id.assign(this->__activity_id.data());
	}

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "join" <<
		"d" << JSON(
			"activityId" << _activity_id
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("joining activity ");
		_log.insert(_log.length(), _activity_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::quit() {
	assertz(this->__is_initiating_master, "quit can only be invoked by the initiating Activity Master, you must create the activity before quiting (use createActivity method) or use participantQuit method instead.", 500, 102);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "quit";

#ifdef MUZZLEY_DEBUG
	{
		string _log("quiting:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::participantQuit() {
	assertz(this->__participant_id != -1, "in order to quit you must have joined first (use joinActivity method)", 500, 105);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "participantQuit" <<
		"d" << JSON(
			"participantId" << this->__participant_id
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("participant quiting:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::participantReady(muzzley::Callback _callback) {
	assertz(this->__participant_id != -1, "in order to be ready you must have joined first (use joinActivity method)", 500, 106);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "ready"
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("participant ready:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::changeWidget(long _participant_id, string _widget, muzzley::Callback _callback) {
	muzzley::JSONObj _options;
	_options <<
		"widget" << _widget;
	this->changeWidget(_participant_id, _options, _callback);
}

void muzzley::Client::changeWidget(long _participant_id, string _widget, muzzley::JSONObj& _params, muzzley::Callback _callback) {
	muzzley::JSONObj _options;
	_options <<
		"widget" << _widget <<
		"params" << _params;
	this->changeWidget(_participant_id, _options, _callback);
}

void muzzley::Client::changeWidget(long _participant_id, muzzley::JSONObj& _options, muzzley::Callback _callback) {
	string _widget;
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "changeWidget" <<
			"d" << _options
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("change widget, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::Callback _callback) {
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "setupComponent" <<
			"d" << JSON(
				"c" << _component <<
				"id" << _component_id <<
				"a" << _action
				)
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("setup component, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::JSONObj& _options, muzzley::Callback _callback) {
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "setupComponent" <<
			"d" << JSON(
				"c" << _component <<
				"id" << _component_id <<
				"a" << _action <<
				"p" << _options
				)
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("setup component, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(long _participant_id, string _type, muzzley::Callback _callback) {
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal to participant ");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(long _participant_id, string _type, muzzley::JSONObj& _data, muzzley::Callback _callback) {
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type <<
			"d" << _data
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal to participant ");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), ":\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(string _type, muzzley::Callback _callback) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(string _type, muzzley::JSONObj& _data, muzzley::Callback _callback) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type <<
			"d" << _data
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendWidgetData(string _widget, string _component, string _event_type, string _event_value) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"t" << 5
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"w" << _widget <<
			"c" << _component <<
			"e" << _event_type <<
			"v" << _event_value
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending widget data:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	this->write(_message);
}

void muzzley::Client::setLoopAssynchronous(bool _assync) {
	this->__is_loop_assynchronous = _assync;
}

const string& muzzley::Client::getActivityId() const {
	return __activity_id;
}

const string& muzzley::Client::getDeviceId() const {
	return __device_id;
}

const string& muzzley::Client::getSessionId() const {
	return __session_id;
}

const long muzzley::Client::getParticipantId() const {
	return __participant_id;
}

bool muzzley::Client::isReplyNeeded(muzzley::JSONObj& _data_received) const {
	muzzley::MessageType _type = (muzzley::MessageType) ((int) _data_received["h"]["t"]);
	return _type == muzzley::RequestInitiatedByEndpoint || _type == muzzley::RequestInitiatedMuzzleyCore;
}

bool muzzley::Client::isAppLoggedin() const {
	return __is_app_loggedin;
}

bool muzzley::Client::isInitiatingMaster() const {
	return __is_initiating_master;
}

bool muzzley::Client::isConnected() const {
	return this->__is_connected;
}

bool muzzley::Client::isLoopAssynchronous() const {
	return __is_loop_assynchronous;
}

bool muzzley::Client::isStaticActivity() const {
	return __is_static_activity;
}

bool muzzley::Client::isUserLoggedin() const {
	return __is_user_loggedin;
}

const muzzley::ParticipantList& muzzley::Client::getParticipants() const {
	return __participants;
}

bool muzzley::Client::handshake(muzzley::Handler _callback) {
	muzzley::JSONObj _message;
	_message <<
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "handshake" <<
		"d" << JSON(
			"protocolVersion" << "1.2.0" <<
			"lib" << "C++ 0.1.0"
			);

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending handshake:\n-> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::sys);
	}
#endif

	return this->write(_message, _callback);
}

void muzzley::Client::process(muzzley::JSONObj& _received, muzzley::EventType* _type) {
	int _interaction = (int) _received["h"]["t"];
	string _action;
	string _signal_action;
	switch (_interaction) {
		// Requests originated at the server or client
		case muzzley::RequestInitiatedByEndpoint:
			// Fall through
		case muzzley::RequestInitiatedMuzzleyCore:
			// Fall through
		case muzzley::Signaling:
			_action.assign((string) _received["a"]);
			if (_action == "signal" && !!_received["d"] && !!!!_received["d"]["a"]) {
				_signal_action.assign((string) _received["d"]["a"]);
			}
			break;
			// Replies to requests initiated here
		case muzzley::ReplyToEndpoint:
			// Fall through
		case muzzley::ReplyToMuzzleyCore:
			pthread_mutex_lock(this->__mtx);
			CallbackQueue::iterator _req_data = this->__queue.find((int) _received["h"]["cid"]);
			pthread_mutex_unlock(this->__mtx);

			if (_req_data != this->__queue.end()) {
				_action.assign(_req_data->second.first);
				if (_req_data->second.second != NULL) {
					(_req_data->second.second)(_received, *this);
				}

				pthread_mutex_lock(this->__mtx);
				this->__queue.erase(_req_data);
				pthread_mutex_unlock(this->__mtx);
			}
			break;
	}

	if (_action == "handshake") {
		*_type = muzzley::Handshake;
	}
	else if (_action == "loginApp") {
		*_type = muzzley::AppLoggedIn;
	}
	else if (_action == "loginUser") {
		*_type = muzzley::UserLoggedIn;
	}
	else if (_action == "create") {
		*_type = muzzley::ActivityCreated;
	}
	else if (_action == "join") {
		*_type = muzzley::ActivityJoined;
	}
	else if (_action == "activityTerminated") {
		*_type = muzzley::ActivityTerminated;
	}
	else if (_action == "participantJoined") {
		*_type = muzzley::__INTERNAL_ParticipantJoined__;
	}
	else if (_action == "participantQuit") {
		*_type = muzzley::ParticipantQuit;
	}
	else if (_action == "signal") {
		if (_signal_action == "ready") {
			*_type = muzzley::ParticipantJoined;
		}
		else if (_signal_action == "changeWidget") {
			// {"c":"ba","e":"release","v":"a","w":"gamepad"}
			*_type = muzzley::ChangeWidget;
		}
		else if (_signal_action == "setupComponent") {
			// {"c":"ba","e":"release","v":"a","w":"gamepad"}
			*_type = muzzley::SetupComponent;
		}
		else if (_signal_action == "" && !!_received["d"]["w"] && !!_received["d"]["c"]) {
			// {"c":"ba","e":"release","v":"a","w":"gamepad"}
			*_type = muzzley::WidgetAction;
		}
		else {
			*_type = muzzley::SignalingMessage;
		}
	}
}

void muzzley::Client::run() {
	for (; true;) {
		try {
			if (!this->isConnected()) {
				this->reconnect();
			}

			if (this->read()) {
				if (this->__message == "h") {
					this->__message.clear();
					continue;
				}

#ifdef MUZZLEY_DEBUG
				{
					string _log("received a message\n<- ");
					_log.insert(_log.length(), this->__message);
					muzzley::log(_log, muzzley::info);
				}
#endif
				switch (this->__op_code) {
					case 0x0: {
						break;
					}
					case 0x01: {
						muzzley::JSONObj _received;
						muzzley::fromstr(this->__message, _received);
						muzzley::EventType _type;
						this->process(_received, &_type);
						this->trigger(_type, _received);
						break;
					}
					case 0x02: {
						break;
					}
					case 0x08: {
						this->disconnect();
						break;
					}
					case 0x09: {
						break;
					}
				}

				this->__message.clear();
				this->__op_code = -1;

			}
		}
		catch (muzzley::AssertionException& e) {
#ifdef MUZZLEY_DEBUG
			{
				string _log("assertion error: ");
				_log.insert(_log.length(), e.description());
				muzzley::log(_log, muzzley::error);
			}
#endif
		}
	}
}

void muzzley::printbits(unsigned char _b) {
	bitset<8> _bs(_b);
	cout << _bs.to_string() << endl << flush;
}

void muzzley::Client::start() {
	pthread_create(this->__thr, 0, &Client::launch, this);
}

void* muzzley::Client::launch(void* _thread) {
	Client* _running = static_cast<Client*>(_thread);
	_running->run();
	return NULL;
}
