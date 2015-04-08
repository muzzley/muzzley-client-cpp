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

//
// You can compile this application with the following command:
// g++ -std=c++0x -I/usr/include/muzzley examples/app.cpp -o myapp -lmuzzley -lpthread
//
// And run it like this:
// ./myapp
// 
// Once running, you can pair your smartphone to this app using
// the mobile muzzley application and the "Activity Id" that is
// written to the screen.
//

#define APP_TOKEN "c1bc8d7d6bc38873" // Get yours at muzzley.com

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
	string _component_id = ${due to public usage of demo channels and to the load balacing infra-strucuture, when using the muzzley:_demo-channel_, replace this with a unique string, for instance you IP address combined with the timestamp};

	// Create the Muzzley client
	muzzley::Client _client;

	// Register listener to be invoked when activity is sucessfully created.
	//
	// Don't bother to store the returned activityId, the Client class already does that,
	// access it through the 'getActivityId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::AppLoggedIn, [ _component_id ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		cout << "App logged in with created with session id " << (string) _data["d"]["sessionId"] << endl << flush;

		muzzley::Subscription _s1;
		_s1.setNamespace("iot");
		_s1.setProfile("muzzley");
		_s1.setChannel("_demo-channel_");
		_s1.setComponent(_component_id);
		_s1.setProperty("status");
		_client.on(muzzley::Published, _s1, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			_data->prettify(cout);
			cout << endl << flush;
			return true;
		});

		muzzley::Subscription _s2;
		_s2.setNamespace("iot");
		_s2.setProfile("muzzley");
		_s2.setChannel("_demo-channel_");
		_s2.setComponent(_component_id);
		_s2.setProperty("intensity");
		_client.on(muzzley::Published, _s2, [ _component_id ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			_data->prettify(cout);
			cout << endl << flush;

			if (_data.isReplyNeeded()) {
				muzzley::Message _m;
				_m.setStatus(true);
				_m.setData(JSON(
					"value" << 125 << 
					"unit" << "lm"
				));
				_client.reply(_data, _m);
				
				muzzley::Subscription _s;
				_s.setNamespace("iot");
				_s.setProfile("muzzley");
				_s.setChannel("_demo-channel_");
				_s.setComponent(_component_id);
				_s.setProperty("intensity");
				_client.off(muzzley::Published, _s);
			}
			return true;			
		});
		return true;
	});

	// Connects the application to the Muzzley server, using the one-step-initialization process.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.initApp(APP_TOKEN);

	return 0;
}
