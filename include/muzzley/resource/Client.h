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

#include <muzzley/json/JSONObj.h>
#include <pthread.h>
#include <muzzley/stream/SocketStreams.h>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#define MUZZLEY_ENDPOINT_PORT 80
#define MUZZLEY_ENDPOINT_PATH "/ws"
//#define MUZZLEY_DEBUG

#ifndef CRLF
#define CRLF "\r\n"
#endif

namespace muzzley {
	
#define N_EVENT_TYPES 15
	enum EventType {
		Connect = 0,
		Handshake = 1,
		ActivityCreated = 2,
		ActivityJoined = 3,
		ActivityTerminated = 4,
		ParticipantJoined = 5,
		ParticipantQuit = 6,
		SignalingMessage = 7,
		ChangeWidget = 8,
		SetupComponent = 9,
		AppLoggedIn = 10,
		UserLoggedIn = 11,
		WidgetAction = 12,
		__INTERNAL_ParticipantJoined__ = 13,
		__INTERNAL_PublishUpdate__ = 14,
		Published = 15,
		Publish = 16
	};
	enum MessageType {
		RequestInitiatedByEndpoint = 1,
		ReplyToEndpoint = 2,
		RequestInitiatedMuzzleyCore = 3,
		ReplyToMuzzleyCore = 4,
		Signaling = 5
	};


	class Subscription : public muzzley::JSONObj {
	public:
		Subscription();
		Subscription(muzzley::JSONObj&);
		virtual ~Subscription();

		virtual string getNamespace();
		virtual string getProfile();
		virtual string getChannel();
		virtual string getComponent();
		virtual string getProperty();

		virtual void setNamespace(string _in);
		virtual void setProfile(string _in);
		virtual void setChannel(string _in);
		virtual void setComponent(string _in);
		virtual void setProperty(string _in);

	};

	class Message : public muzzley::JSONObj {
	public:
		Message();
		Message(muzzley::JSONObj&);
		virtual ~Message();

		virtual muzzley::MessageType getMessageType();
		virtual string getCorrelationID();
		virtual string getParticipantID();
		virtual string getChannelID();
		virtual string getAction();
		virtual muzzley::JSONObj& getData();
		virtual bool getStatus();
		virtual string getStatusMessage();
		virtual void getSubscriptionInfo(muzzley::Subscription&);

		virtual void setMessageType(muzzley::MessageType _in);
		virtual void setCorrelationID(string _in);
		virtual void setParticipantID(string _in);
		virtual void setChannelID(string _in);
		virtual void setAction(string _in);
		virtual void setData(muzzley::JSONObj& _in);
		virtual void setStatus(bool _in);
		virtual void setStatusMessage(string _in);

		virtual bool isError();
		virtual bool isRequest();
		virtual bool isReponse();
		virtual bool isReplyNeeded();

	};

	class Client;
	typedef std::function<bool(muzzley::Message&, muzzley::Client&)> Handler;
	typedef Handler Callback;
	typedef map<int, pair<string, Callback> > CallbackQueue;
	typedef map<long, string> ParticipantList;
	typedef map< string, Callback > SubscriptionStack;
	typedef map< string, vector < string > > SubscriptionChannels;

	class Client {
	public:
		Client();
		virtual ~Client();

	#if __cplusplus >= 201103L
		virtual void on(muzzley::EventType _type, muzzley::Handler _handler) final;
		virtual void trigger(muzzley::EventType _type, muzzley::Message& _data) final;

		virtual void on(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Callback _callback) final;
		virtual void off(muzzley::EventType _type, muzzley::Subscription& _from_property) final;
		virtual void trigger(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Message& _payload, muzzley::Callback _callback = nullptr) final;

	#endif
		virtual void start() final;

		virtual void connectApp(string _app_token, string _activity_id = "");
		virtual void connectUser(string _user_token, string _activity_id);
		virtual void loginApp(string _app_token) final;
		virtual void loginUser(string _user_token) final;
		virtual void initApp(string _app_token, string _activity_id);
		virtual void initUser(string _user_token, string _activity_id);

		virtual void createActivity(string _activity_id = "");
		virtual void joinActivity(string _activity_id = "");
		virtual void quit();
		virtual void participantQuit();

		virtual void participantReady(muzzley::Callback _callback = nullptr);
		virtual void changeWidget(long _participant_id, string _widget, muzzley::Callback _callback = nullptr);
		virtual void changeWidget(long _participant_id, string _widget, muzzley::JSONObj& _params, muzzley::Callback _callback = nullptr);
		virtual void changeWidget(long _participant_id, muzzley::JSONObj& _options, muzzley::Callback _callback);
		virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::Callback _callback = nullptr);
		virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::JSONObj& _options, muzzley::Callback _callback = nullptr);

		virtual void sendSignal(long _participant_id, string _type, muzzley::Callback _callback = nullptr);
		virtual void sendSignal(long _participant_id, string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = nullptr);
		virtual void sendSignal(string _type, muzzley::Callback _callback = nullptr);
		virtual void sendSignal(string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = nullptr);

		virtual void sendWidgetData(string _widget, string _component, string _event_type, string _event_value);

		virtual bool connect(string _host, uint16_t _port, string _path) final;
		virtual void disconnect() final;
		virtual void reconnect() final;
		virtual bool read() final;
		virtual bool write(muzzley::Message& _data, muzzley::Callback _callback = nullptr) final;
		virtual bool reply(muzzley::Message& _data_received, muzzley::Message& _reply) final;

		void setLoopAssynchronous(bool _assync);

		const string& getActivityId() const;
		const string& getDeviceId() const;
		const string& getSessionId() const;
		const string& getChannelId() const;
		const long getParticipantId() const;
		const ParticipantList& getParticipants() const;

		bool isReplyNeeded(muzzley::Message& _data_received) const;
		bool isAppLoggedin() const;
		bool isInitiatingMaster() const;
		bool isConnected() const;
		bool isLoopAssynchronous() const;
		bool isStaticActivity() const;
		bool isUserLoggedin() const;

		const string& getEndpointHost() const;
		void setEndpointHost(string _endpoint_host);

	private:
		std::vector<muzzley::Handler> __handlers[N_EVENT_TYPES];
		long long __serial;
		int __op_code;

		string __endpoint_host = "platform.office.muzzley.com";

		string __app_token;
		string __user_token;
		string __activity_id;
		string __message;
		string __session_id;
		string __device_id;
		string __channel_id;
		long __participant_id;

		muzzley::socketstream __channel;

		CallbackQueue __queue;
		ParticipantList __participants;
		SubscriptionStack __stack;
		SubscriptionChannels __namespaces;


		bool __is_static_activity;
		bool __is_connected;
		bool __is_to_reconnect;
		bool __is_loop_assynchronous;
		bool __is_app_loggedin;
		bool __is_user_loggedin;
		bool __is_initiating_master;
		bool __is_running;
		bool __is_one_step_initialization;
		bool __has_handshake;

		pthread_mutex_t* __mtx;
		pthread_t* __thr;
		pthread_mutexattr_t __attr;

		bool handshake(muzzley::Handler _callback);
		bool process(muzzley::Message& _received, muzzley::EventType* _type);
		void subscribe(muzzley::Subscription& _to_property, muzzley::Callback _callback);
		void unsubscribe(muzzley::Subscription& _from_property);
		void publish(muzzley::Subscription& _to_property, muzzley::Message& _payload, muzzley::Callback _callback = nullptr);


		void run();
		static void* launch(void* thread);

	};

	void printbits(unsigned char _b);
}
