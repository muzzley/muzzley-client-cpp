# Muzzley C++ Client

This repo contains the code for the [muzzley][muzzley_homepage] C++ client library.

# DEPENDENCIES

This library depends on:

- GCC 4.5+
- pthread
- autotools
- libtool

# VERSIONING

Current version: [0.0.2][changelog] - ***Please read since this version contains breaking changes***

# INSTALL

## Linux & OSX

To install this library reproduce the following steps on a terminal:

> $ curl -Lk -o muzzley-client-cpp.zip https://github.com/muzzley/muzzley-client-cpp/archive/master.zip

> $ unzip muzzley-client-cpp.zip

> $ cd muzzley-client-cpp-master/

> $ autoreconf -fi

> $ ./configure --prefix=/usr

> $ make

> $ sudo make install

The library will be located in `/usr/lib` and the header files (\*.h) will be located in `/usr/include`.  

If you want another target instalation directory, just pass it to the *./configure --prefix=[your prefix]* instalation command. Just don't forget to pass a *-I[your prefix]* flag to your compilation command.

### Enabling HTTP(s) client support

Being the purpose of this library to interact with the Muzzley platform, you may need to make HTTP request to the Muzzle API. Hence, an HTTP(s) client is supplied. To build and install the client as a part of the library, you must provide the ***--enable-http*** argument to the *configure* script:

> $ ./configure --prefix=/usr --enable-http

The client depends on the [OpenSSL][open_ssl] library, more specifically, on the **libssl** and **libcrypto** libraries and development packages.

See se ***HTTP(s) support*** section, below.

### Enabling message logging support

If you want to inspect the messages being exchanged by the library, you must provide the ***--enable-logs*** argument to the *configure* script:

> $ ./configure --prefix=/usr --enable-logs

# EXAMPLE APPLICATIONS

In the `Examples` folder, you can find several examples of muzzley-enabled applications for the Intel Galileo and other generic platforms.

* **`app.cpp`**: A generic muzzley-enabled application.
* **`galileo_app.cpp`**: A Intel Galileo specific example that can be used to turn an LED on and off using your smartphone.
* **`user.cpp`**: An example of how to connect to an activity as a user and thus simulate being the muzzley smartphone application.
* **`app_pubsub.cpp`** and **`user_pubsub.cpp`**: An example of how to use the Pub/Sub communication pattern.

To compile and run the Galileo example, for instance, you'd execute the following commands:

> $ g++ -std=c++0x examples/galileo_app.cpp -o myapp -lmuzzley -lpthread

> $ ./myapp

Once running, the application will output the `Activity Id` code that can be used to pair your smartphone with it using the mobile muzzley application ([Android](https://play.google.com/store/apps/details?id=com.muzzley), [iPhone](https://itunes.apple.com/us/app/muzzley/id604133373), [Windows Phone](http://www.windowsphone.com/s?appid=b909cf49-9336-403c-b25d-039b5ec1bd4a&signin=true)).

# LINKING AND INCLUDING

You should include the `muzzley.h` header file in your program, so that you can access the Muzzley classes. 

Assuming that your main.cpp C++ file looks something like this:

```
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley/muzzley.h>

using namespace std;
using namespace __gnu_cxx;

int main(int argc, char* argv[]) {

  muzzley::Client _client;

  _client.on(muzzley::SignalingMessage, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
    ...
    return true;
  });
  ...

  _client.initApp("YOUR-APP-TOKEN-HERE");

  return 0;
}
```

A compilation command could look something like this:

> $ g++ -std=c++0x main.cpp -o mymuzzley -lpthread -lmuzzley

# LAMBDA FUNCTIONS

This library uses one of the most exciting features in C++11 standard, the ***Lambda-function***s. 

The ***Lambda-function*** declaration allows the definition of inline functions anywhere in the code,  a lot like every other coding-ninja-favourite languages such as JavaScript, Python, etc. This makes the event handling programming more coeherent with nowadays coding habits and makes C++ a less rigid OO language and lot friendlier for programming paradigms that fall outside OO.

Let's look at an example:

    muzzley::Client _client;

    _client.on(muzzley::SignalingMessage, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      ...
      return true;
    });

[Here][lambda_functions] is an excellent article on the subject.

# MAIN PROGRAM LOOP

Your program's interaction with this library is centered in 3 simple and well defined areas:

- *muzzley::Client* class instance declaration (initializes the library)
- event register (call your actions when the library receives a message) or callback register (pass a callback to be invoked when receiving a response to a given request)
- connecting the application or the user to the Muzzley server (pass an application or user token to the library and start the messaging loop)  

You can register handlers for the following events:

    enum EventType {
      ActivityCreated, // when an activity is created with success
      ActivityJoined,  // when a response to joinActivity is finished successfully
      ActivityTerminated,  // when an Activity Master quits
      ParticipantJoined,  // when a Participant (another app or user) joins an activity
      ParticipantQuit,  // when a Participant quits
      SignalingMessage,  // when a Signaling message is received
      ChangeWidget, // when a change widget request is received
      SetupComponent, // when a setup component request is received
      WidgetAction, // when a widget action is received
      Published, // when a message is published to a given Pub/Sub channel
      Publish // to publish a message to a given Pub/Sub channel
    };

An event register could look something like this:

    muzzley::Client _client;

    _client.on(muzzley::SignalingMessage, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      ...
      return true;
    });

Callback register also uses ***Lambda-function***s, It could look something like this:

    muzzley::Client _client;

    ...

    _client.changeWidget(participant_id, _widget_opts, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      cout << "Widget successfully changed" << endl << flush;
      return true;
    });

When you connect and initialize the application or the user to the Muzzley server (by invoking either ***initApp*** or ***initUser*** methods), the messaging loop starts. You can either run this loop synchronously or assynchronously, depending wether you want to keep control of the program loop or not.

You control this by invoking the *muzzley::Client::setLoopAssynchronous(bool _assync)*. Passing *true* value will give you back the control of the program by lanuching a thread and returning from the ***initApp*** or ***initUser*** methods. Not invoking the method or passing *false* value, will keep the program execution inside the library, never returning from the ***initApp*** or ***initUser*** methods.

Hence, a basic program could look something like this:

```
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley/muzzley.h>

using namespace std;
using namespace __gnu_cxx;

int main(int argc, char* argv[]) {

  // Create the Muzzley client
  muzzley::Client _client;

  // Register listener to be invoked when a participant joins our activity.
  //
  // Don't bother to store the returned activityId, the Client class already does that,
  // access it through the 'getActivityId' method.
  //
  // Return 'false' if you want to stop other listeners from being invoked.
  _client.on(muzzley::ParticipantJoined, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
    muzzley::JSONObj _s_data;
    _s_data <<
      "arg1"<< "value1" <<
      "arg2"<< "value2";

    _client.sendSignal("myAction", _s_data, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      cout << "Signal accepted by the user" << endl << flush;
    });
    return true;
  });

  // Register listener to be invoked when app receives a signal message.
  //
  // Return 'false' if you want to stop other listeners from being invoked.
  _client.on(muzzley::SignalingMessage, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
    cout << (string) _data[ "d" ]["a"] << endl << flush;
    return true;
  });

  // Connects the application to the Muzzley server.
  //
  // It will start the application loop synchronously,
  // i.e. the program will not execute anything below this line.
  _client.connectApp("YOUR-APP-TOKEN-HERE");

  return 0;
}
```

# REPLYING TO A MESSAGE

Sometimes you'll need to reply to a received message. In order to do so, you may use the *reply* method:
	
```
virtual bool reply(muzzley::JSONObj& _data_received, muzzley::JSONObj& _reply) final;
```

This method receives the original received message and the data you want to reply with. The reply message fields are:

- **s**: A boolean representing whether the authentication request was successful *(mandatory)*
- **m**: A textual message explaining the result of the operation *(optional)*
- **d**: The aditional data *(optional)*

The *reply* method returns *true* if a message were sent and *false* if not (if the received message was of a type that doesn't require a reply, for instance). 

Hence, replying, for instance, to a Signaling Message could look like this:

    _client.on(muzzley::SignalingMessage, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      if (_data.isReplyNeeded()) {
        _m.setStatus(true);
        _m.setStatusMessage("this is a testing signal so is always successful!");
        _m.setData(JSON(
          "w" << "400" <<
          "h" << "300"
        ));
        _client.reply(_data, _m);
        cout << "great, replied to a Signal Message" << endl << flush;
      }
      return true;
    });

# PUB/SUB COMMUNICATION PATTERN

The [Pub/Sub message pattern][wiki_pubsub] is a powerful communication mechanism that allows you to send and/or receive messages without knowing the communication actors and, therefore, not having to deal with the problems of sending messages, directly, to specific peers.

For further reading on this pattern, read the [wiki page][wiki_pubsub].

## Subscribing to a channel

In order to subscribe for the messages of a given channel, an event callback must be registered for the *muzzley::Published* event:

```
    muzzley::Subscription _s1;
    _s1.setNamespace("iot");
    _s1.setProfile("cb17073b-9428-4f55-9a22-26a008c0bf4e");
    _s1.setChannel("2183hroqw89");
    _s1.setComponent("switch");
    _s1.setProperty("status");
    _client.on(muzzley::Published, _s1, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      ...
      return true;
    });
```

The **muzzley::Subscription** class is used to pass the channel identifying information. The initializing values for this class should be the same for the subscriber and the publisher.

The callback will be invoked whenever a message is published to the given channel. 

## Publishing to a channel

In order to publish to a given channel, an event must be triggered for the *muzzley::Publish* event:

```
    muzzley::Subscription _s1;
    _s1.setNamespace("iot");
    _s1.setProfile("cb17073b-9428-4f55-9a22-26a008c0bf4e");
    _s1.setChannel("2183hroqw89");
    _s1.setComponent("bulb");
    _s1.setProperty("intensity");

    muzzley::Message _m1;
    _m1.setData(JSON(
      "io" << "w" <<
      "data" << JSON(
        "value" << 125 << 
        "unit" << "lm"
      )
    ));
    _client.trigger(muzzley::Publish, _s1, _m1);
```

The **muzzley::Message** class is used to pass on the payload to be published to the given channel.

The **muzzley::Subscription** class is used to pass the channel identifying information. The initializing values for this class should be the same for the subscriber and the publisher.

## Publishing to a channel and expecting a response

Altough it falls a bit outside the Pub/Sub messaging pattern, this library provides the functionality for a message to be returned by a given subscriber (usefull for making request for a given property value, for instance), allowing you to use the Pub/Sub pattern to make RPC calls.

All you have to do is to pass a callback to the publish event triggering:

```
    muzzley::Subscription _s1;
    _s1.setNamespace("iot");
    _s1.setProfile("cb17073b-9428-4f55-9a22-26a008c0bf4e");
    _s1.setChannel("2183hroqw89");
    _s1.setComponent("bulb");
    _s1.setProperty("intensity");

    muzzley::Message _m1;
    _m1.setData(JSON(
      "io" << "w" <<
      "data" << JSON(
        "value" << 125 << 
        "unit" << "lm"
      )
    ));
    _client.trigger(muzzley::Publish, _s1, _m1, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
      ...
      return true;
    }); 
```

The **muzzley::Message** class is used to pass on the payload to be published to the given channel.

The **muzzley::Subscription** class is used to pass the channel identifying information. The initializing values for this class should be the same for the subscriber and the publisher.

The callback will be invoked if some subscriber responds to the publish.

# MAIN CLASSES

There are two classes that you have to learn how to use in order to adequatly use this library:

- **muzzley::Client**: will allow you to interact with the Muzzley server, as well as with other applications and users.
- **muzzley::JSONObj**; will allow you to easly manage data, either when sending it or receiving it.
- **muzzley::Message**; extends from *muzzley::JSONObj* and abstract the communication protocol.
- **muzzley::Subscription**; extends from *muzzley::JSONObj* and abstract the pub-sub protocol channel property definitions.


## muzzley::Client

This class aggregates a set of methods that will allow the interaction between your application and the Muzzley server. Basically, implements the Muzzley Message Protocol.

This class methods are organized in the following way:

### Connection & Authentication

    // Intialiazition of the protocol version 1.2
    // Only need to provide `_activity_id` when using a static activity token as configured at muzzley.com
    virtual void connectApp(string _app_token, string _activity_id = "");
    virtual void connectUser(string _user_token, string _activity_id);

    // Intialiazition of the protocol version 2.0
    virtual void initApp(string _app_token);
    virtual void initUser(string _user_token);

### Event registering

    virtual void on(muzzley::EventType _type, muzzley::Handler _handler) final;

### Event registering for Pub/Sub

    virtual void on(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Callback _callback);
    virtual void off(muzzley::EventType _type, muzzley::Subscription& _from_property);
    virtual void trigger(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Message& _payload, muzzley::Callback _callback = nullptr);

**NOTE**: *_type* must be *muzzley::Published* or *muzzley::Publish*

### Message Handling

    virtual bool reply(muzzley::Message& _data_received, muzzley::Message& _reply) final;

### Activity

    virtual void quit();
    virtual void participantQuit();

### Signaling

    virtual void changeWidget(long _participant_id, string _widget, muzzley::Callback _callback = NULL);
    virtual void changeWidget(long _participant_id, string _widget, muzzley::JSONObj& _options, muzzley::Callback _callback = NULL);
    virtual void changeWidget(long _participant_id, muzzley::JSONObj& _options, muzzley::Callback _callback);
    virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::Callback _callback = NULL);
    virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::JSONObj& _options, muzzley::Callback _callback = NULL);
    virtual void sendSignal(long _participant_id, string _type, muzzley::Callback _callback = NULL);
    virtual void sendSignal(long _participant_id, string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = NULL);
    virtual void sendSignal(string _type, muzzley::Callback _callback = NULL);
    virtual void sendSignal(string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = NULL);
    virtual void sendWidgetData(string _widget, string _component, string _event_type, string _event_value);

### Getters and Setters

    void setLoopAssynchronous(bool _assync);
    const string& getActivityId() const;
    const string& getDeviceId() const;
    const string& getSessionId() const;
    const long getParticipantId() const;
    const ParticipantList& getParticipants() const;

### Flag testing

    bool isReplyNeeded(muzzley::Message& _data_received) const;
    bool isAppLoggedin() const;
    bool isInitiatingMaster() const;
    bool isConnected() const;
    bool isLoopAssynchronous() const;
    bool isStaticActivity() const;
    bool isUserLoggedin() const;

## muzzley::JSONObj

This class stores JSON based message and aggregates a set of methods that allow you to access, in a fashionable manner, all the attributes and values of the object. It also allows you to construct a JSON based message from scratch.

### Smart pointers

The *muzzley::JSONObj* class is a smart pointer and that means that ou don't have to bother yourself with memory issues, once your done with your *muzzley::JSONObj* object just let it go out of it's context and thats it.

To obtain the reference for the pointed object, you may use **\*** or **->**. This means that some of *muzzley::JSONObj* methods are accessed through **->** and others (the ones that reference the smart pointer itself) are accessed through **.**.

For further reading on smart pointers, please read [this C++ Reference section][cpp_smart_pointers].

### '<<'' operator

This operator allows you to add attributes to your object:

    muzzley::JSONObj _o;
    _o << "name" << "Mr Muzzley";
    _o << "serial" << 123;
    _o << "sorting_field" << "name";

or

    muzzley::JSONObj _o;
    _o << 
      "name" << "Mr Muzzley" <<
      "serial" << 123 <<
      "sorting_field" << "name";
    // this one is more JSON like

To add a JSON array, use the *muzzley::JSONArr* class:

    muzzley::JSONArr _a;
    _a << 123 << 345 << 67 << 78;

    muzzley::JSONArr _b;
    _b << "lions" << 345 << "horses" << 78;

    muzzley::JSONObj _o;
    _o << 
      "name" << "Mr Muzzley" <<
      "serial" << 123 <<
      "sorting_field" << "name" <<
      "numbers" << _a <<
      "animal_numbers" << _b;

You can use the ***JSON*** and ***JSON_ARRAY*** macros to build an object in one line:

    muzzley::JSONObj _o = JSON(
      "name" << "Mr Muzzley" <<
      "serial" << 123 <<
      "sorting_field" << "name" <<
      "numbers" << JSON_ARRAY( 123 << 345 << 67 << 78 ) <<
      "animal_numbers" << JSON_ARRAY( "lions" << 345 << "horses" << 78 ) <<
      "location" << JSON(
        "city" << "Lisbon" <<
        "country" << "Portugal"
      )
    );

### '[]' operator and casting operators

To access the *muzzley::JSONObj* values you use the *[]* operator (or a chain of *[]* operators). This operator accept either a string or a non-negative integer and returns the value associated with the given key or *false* (it's like an Hash map).

Casting operators are also provided, in order for you to convert the attribute value into the data type that best suits you:

    operator string();
    operator bool();
    operator int();
    operator long();
    operator long long();
    operator unsigned int();
    operator size_t();
    operator double();
    operator time_t();

So, given the above example, you could use the following code: 

    int _serial = (int) _o["serial"];
    int _first_number = (int) _o[ "numbers" ][0];
    string _country = (string) _o["location"]["country"];

### Iterating over *muzzley::JSONObj* properties

Since the *muzzley::JSONObj* is a smart pointer that references an extended *std::map*, iterating over it's properties follows the STL containers standards.

For instance, given the following object initialization:

    muzzley::JSONObj _o = JSON(
      "name" << "Mr Muzzley" <<
      "serial" << 123 <<
      "sorting_field" << "name" <<
      "numbers" << JSON_ARRAY( 123 << 345 << 67 << 78 ) <<
      "animal_numbers" << JSON_ARRAY( "lions" << 345 << "horses" << 78 ) <<
      "location" << JSON(
        "city" << "Lisbon" <<
        "country" << "Portugal"
      )
    );

one could iterate over this object, using standard techniques;
  
    for (auto _field : *_o) { // dont forget the * since you want to iterate over the pointed object
        cout << " field named " << _field.first << " has the following value " << _field.second << endl << flush;
    }

To learn more about *std::map* and map iterators, read [this C++ Reference section][cpp_map].

### Iterating over *muzzley::JSONArr* properties

Since the *muzzley::JSONArr* is a smart pointer that references an extended *std::vector*, iterating over it's properties follows the STL containers standards.

For instance, given the following object initialization:

    muzzley::JSON _a = JSON_ARRAY( "lions" << 345 << "horses" << 78 );

one could iterate over this object, using standard techniques;
  
    for (auto _field : *_a) { // dont forget the * since you want to iterate over the pointed object
        cout << " array element is " << _field << endl << flush;
    }

or 

    for (size_t _i = 0; _i != _a->size(); _i++) { 
        cout << " array element " << _i << " is " << _a[_i] << endl << flush;
    }

To learn more about *std::vector* and vector iterators, read [this C++ Reference section][cpp_vector].

## muzzley::Message

This class stores JSON based message, since it extende *muzzley::JSONObj*, and aggregates a set of helper methods that allow you to access, in a fashionable manner, the Muzzley messaging protocol relevant fields.

This class methods are organized in the following way:

### Getters

    virtual muzzley::MessageType getMessageType();
    virtual string getCorrelationID();
    virtual string getParticipantID();
    virtual string getChannelID();
    virtual string getAction();
    virtual muzzley::JSONObj& getData();
    virtual bool getStatus();
    virtual string getStatusMessage();
    virtual void getSubscriptionInfo(muzzley::Subscription&);

### Setters

    virtual void setMessageType(muzzley::MessageType _in);
    virtual void setCorrelationID(string _in);
    virtual void setParticipantID(string _in);
    virtual void setChannelID(string _in);
    virtual void setAction(string _in);
    virtual void setData(muzzley::JSONObj& _in);
    virtual void setStatus(bool _in);
    virtual void setStatusMessage(string _in);

### Testers

    virtual bool isError();
    virtual bool isRequest();
    virtual bool isReponse();
    virtual bool isReplyNeeded();

## muzzley::Subscription

This class stores JSON based message, since it extends *muzzley::JSONObj*, and aggregates a set of helper methods that allow you to access, in a fashionable manner, the Muzzley Pub/Sub communication pattern channel subscription.

This class methods are organized in the following way:

### Getters

    virtual string getNamespace();
    virtual string getProfile();
    virtual string getChannel();
    virtual string getComponent();
    virtual string getProperty();

### Setters

    virtual void setNamespace(string _in);
    virtual void setProfile(string _in);
    virtual void setChannel(string _in);
    virtual void setComponent(string _in);
    virtual void setProperty(string _in);

# SUPPORT CLASSES

## HTTP(s) support

This library ships whit a built-in HTTP(s) client. It's composed by:

* an HTTP parser for both requests and responses
* ***muzzley::sslsocketstream***, which is an SSL socket stream (all encryption/decryption and certificate operations are delegated to OpenSSL's **libssl** and **libcrypto**)
* ***muzzley::HTTPReq***, which is an *[std::share_ptr][cpp_smart_pointers]* to an HTTP request object 
* ***muzzley::HTTPRep***, which is an *[std::share_ptr][cpp_smart_pointers]* to an HTTP response object 

This classes usage is pretty self explanatory by example:

```
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley/muzzley.h>
#include <muzzley/parsers/http.h>
#include <muzzley/stream/SSLSocketStreams.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

int main(int argc, char* argv[]) {

  // Instantiate an HTTP(s) socket stream
  muzzley::sslsocketstream _ssl;
  // connect to server with muzzley::sslsocketstream::open(host, port)
  _ssl.open("api.muzzley.com", 443);

  // Instantiate an HTTP request object
  muzzley::HTTPReq _req;
  // set HTTP request method
  _req->method(muzzley::HTTPPost);

  // set HTTP request server path
  _req->url("/");

  // Instantiate a string with some body part
  string _body_part("{ \"name\" : \"HTTP Client\", id : 1 }");

  // set HTTP "Host" header
  _req->header("Host", "api.muzzley.com");
  // set HTTP "Content-Type" header
  _req->header("Content-Type", "application/json");
  // set HTTP "Content-Length" header
  _req->header("Content-Length", std::to_string(_body_part.length()));

  // set the message body
  _req->body(_body_part);

  // Send the message
  _ssl << _req << flush;

  // Instantiate an HTTP response object
  muzzley::HTTPRep _rep;
  // Read the message from the stream
  _ssl >> _rep;
  // test the message status
  if (_rep->status() == muzzley::HTTP200) {
    // Print the value of a message header
    cout << "Received a reply with a " << _rep->header("Content-Length") << " bytes of body length" << endl << flush;
    // Print the body of the message
    cout << _rep->body() << endl << flush;
  }

  return 0;
}
```

[muzzley_homepage]: https://www.muzzley.com
[lambda_functions]: http://www.cprogramming.com/c++11/c++11-lambda-closures.html
[changelog]: CHANGES.md
[cpp_smart_pointers]: http://en.cppreference.com/w/cpp/memory/shared_ptr
[cpp_map]: http://en.cppreference.com/w/cpp/container/map
[cpp_vector]: http://en.cppreference.com/w/cpp/container/vector
[wiki_pubsub]: http://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern
[open_ssl]: https://github.com/openssl/openssl
