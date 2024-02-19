# General Documentation about working with GDK cpp
Here's a list of useful information about GDK structures and how to work with them.

## `GA_json`
this structure is a string representation of a json object and it is used to pass parameters to all GDK functions.
GDK offers a variety of functions to convert a string to a `GA_json` struct and vice-versa.
So for example using the nlohmann json library we can create the json object, dump it to string and pass it to `GA_convert_string_to_json`

```cpp
GA_json *netParams;
json obj;
// populate the json object
obj["name"] = "electrum-liquid";
obj["user_agent"] = "test-app";
obj["use_tor"] = false;
// convert json string to GA_json
utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &netParams));
// call a GDK function
utils::wrapCall(GA_connect(session, netParams));

// destroy the GA_json
GA_destroy_json(netParams);

```

## `utils::wrapCall` utility
Every call to GDK functions should be wrapped in a `utils::wrapCall` function. This function will check the return value of the GDK function and throw an exception if the return value is not `GA_OK` getting the error informations automatically using `GA_get_thread_error_details`.

## `GA_session`
This is the struct that will hold the session informations. It is created using `GA_create_session` and destroyed using `GA_destroy_session`.
It is stored as a member of the `GdkHostObject` class and it is used to call all GDK functions that require a session.

## `GA_auth_handler`
This is the struct that holds the reference to an authenticated function/network call. Use it wrapped `utils::resolve(call)`.

```cpp
GA_auth_handler *call; // create the auth handler
utils::wrapCall(GA_register_user(session, hw_device_json, details_json, &call)); // make the authenticated net call
json res = utils::resolve(call); // resolve the call using the utility function

```

## `GA_notification_handler`
This is the function handler that will dispatch the notifications from GDK to the application.
This notification handler takes in input a void pointer to use as "context".
We can pass there our `GdkHostObject` instance and use it to dispatch the notifications to the application.
This notification handler will be called on a separate thread because it receives events from a GDK internal websocket connected to GDK servers.

To call safely this function we need to use the same logic that we are applying to the async functions (reference to the `about_jsi` document for more information):
- The notification is received
- A map of `eventName => jsi::Function` matches the notification name to the function that has to be called, these functions are defined by the application
- If there is a matching handler we need to get the runtime and the callInvoker reference from `GdkHostObject`
- parse the event json
- lock and gain access to the JS runtime using the CallInvoker
- call the function with the parsed json as argument
- js receives the events and can handle them

## Useful links
- https://gdk.readthedocs.io/en/release_0.69.0
