# Version 0.0.2

## Apr 07, 2015

- Added **muzzley::Reconnect** event, in order to determine if the library should try to reconnect when socket hangup is detected
- Added ***rt*** library dependency, for timer-callback utilities (used in reconnect feature)

## Jan 25, 2015

- SSL socket channels became the default for WebSocket connection to the **muzzley** cloud
- Added compilation parameter '--disable-ssl' to create a non-secure channel 

## Dec 28, 2014

- Escaped chacters in JSON string
- Corrected a bug in the JSON parser
- Corrected a compilation problem on JSON parser
- Resolved is_open check on streams

## Nov 18, 2014

- Throwing **muzzley::AssertionException** when sending signaling message with protocol version 2.0 (after initApp/initUser)
- Throwing **muzzley::AssertionException** when sending pub/sub message with protocol version 1.2 (after connectApp/connectUser)
- Added **muzzley::Message** and **muzzley::Subscription** classes, for protocol abstraction
- Implemented the Pub/Sub communication pattern support
- Corrected a memory leak with class muzzley::JSONObj
- Changed muzzley::JSONObj hierarchy, now extends from *std::share_ptr*
