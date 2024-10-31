# General Documentation about working with JSI
The JSI is an interface that allows a JavaScript object to hold a reference to C++ and vice-versa.

Once an object has a reference to the other one, it can directly invoke methods on it. So, for example, a C++ object can now ask a JavaScript object to execute a method in the JavaScript world and viceversa.

## Why
In older React Native versions Native Module were developed creating a bridge between JavaScript and Native Code using Swift/Objective-C or Java/Kotlin. This implicated that the native code had to be developed in at least two different languages and then integrated with the JavaScript code.

With JSI we can use only one source code for both platform with the only difference being a few files that hold platform specific logics.

## Project structure
```
react-native-gdk
├── ios
│   ├── NativeGdk.h
│   └── NativeGdk.mm
├── react-native-gdk.podspec
├── android
│   ├── CMakeLists.txt
│   ├── cpp-adapter.cpp
│   ├── build.gradle
│   └── src/main/java/com
│       ├── blockstream/libgreen_gdk
│       │   └── GDKJNI.java
│       └── gdk
│           ├── GdkModule.java
│           └── GdkPackage.java
└── cpp
    ├── GdkHostObject.cpp
    ├── ThreadPool.cpp
    ├── json.cpp
    └── utils.cpp


```
`ios`
- `NativeGdk.h` and `NativeGdk.mm` are the files that holds the native code for iOS. These expose the `install` native method that will be called from JavaScript and will create the GDK host object
- `react-native-gdk.podspec` is a file that tells `xcodebuild` how to compile the `cpp` folder, `ios` folder and eventually link external libraries like *gdk prebuilts*
- `libgreen_gdk_full.xcframework` is the prebuilt library that holds the GDK code compatible with iOS arm64 (both simulator and phisical device) and x86_64 architectures

`android`
- `GDKJNI.java` is a file only needed to make the GDK prebuilts work correctly. We can ignore that.
- `GdkModule.java` and `GdkPackage.java` are the files that holds the native code for Android. These expose the `install` native method that will be called from JavaScript and will create the GDK host object
- `cpp-adapter.cpp` holds JNI specific code that bridge communincation between Java and C++ code
- `CMakeLists.txt` is a file that tells `ndk-build` how to compile the `cpp` folder and eventually link external libraries like *gdk prebuilts*
- `libgreen_gdk-android` is the prebuilt library that holds the code compatible with Android arm64-v8a, x86_64, armeabi-v7a and x86 architectures

`cpp`
- Contains all the non platform specific code, the core of the application that will be compiled for both platforms
- `GdkHostObject.cpp` is the host object that will be created in the native runtime and exposed to JavaScript, its methods will be callable from JS
- `utils.cpp` contains some utility functions to semplify the workflows
- `ThreadPool.cpp` implementation of a thread pool used to execute async tasks,  source here https://github.com/OP-Engineering/op-sqlite/blob/main/cpp/ThreadPool.cpp
- `json.cpp` contains an implementation of JSON objects in cpp, source here https://github.com/nlohmann/json


## JSI Capabilities
JSI allows to create a host object that can be exposed to JS and can be used to call native methods from JS.
The host object is a cpp class that inherits from `jsi::HostObject` and implements the methods that will be exposed to JS.
To the host object can be further extended with other members and methods that will facilitate the communication between JS and native code.

Then in the native part of iOS and Android (`NativeGdk.mm` and `cpp-adapter.cpp`) we create the shared pointer to the host object that will be shared with the JS runtime by moving it in the JS global object.


JSI can also allow the usage of JS structures and functions direclty in the cpp code:
```cpp

std::string stringify(jsi::Runtime &runtime, const jsi::Value &value) {
  jsi::Object globalObject = runtime.global(); // get the reference of the global JS object

  jsi::Object jsonObject = globalObject.getProperty(runtime, "JSON").getObject(runtime); // gets JSON from global object

  // this corresponds as doing stringifyFunction = JSON.stringify
  jsi::Function stringifyFunction = jsonObject.getProperty(runtime, "stringify").getObject(runtime).asFunction(runtime); // gets the stringify function from JSON object

  // this corresponds as doing jsonString = JSON.stringify(value)
  jsi::Value jsonString = stringifyFunction.call(runtime, value);
  return jsonString.getString(runtime).utf8(runtime);
}
```
or even:
```cpp

// this can log message from the native code directly to the JS console in metro server while developing using the `console.log` function
int consoleLog(jsi::Runtime& runtime, std::string msg) {
  std::string msgCopy(msg);
  jsi::Object globalObject = runtime.global();

  // Access the console object from the global object
  jsi::Object consoleObject = globalObject.getProperty(runtime, "console").getObject(runtime);

  // Get the console.log function
  jsi::Function consoleLog = consoleObject.getProperty(runtime, "log").getObject(runtime).asFunction(runtime);

  // Call console.log with arguments
  consoleLog.call(runtime, msgCopy);
  return 0;
}

```

## About async functions
In JSI it is quite immediate to implement regular syncronous functions:
- define the function in the host object using `jsi::Function::createFromHostFunction` that takes as argument a cpp lambda function that will be executed when the function is called from JS.
- when the function is called cpp takes control of the JS runtime
- executes the function
- returns the result to JS releasing the JS runtime back to the application

This is not ideal for long running tasks or network request because the UI remains blocked until the function returns its result.
There is also a problem with delegating long running tasks to other cpp threads, in fact by design JSI does not allow accessing directly JS runtime in other threads because it can result in unexpected behaviours, crashes or race conditions.

To avoid this we need to use a different approach:
- we need to make sure that the function with the long running task returns immediately a `Promise` to JS before even starting the task
- Use a `ThreadPool` to queue and execute the long running task in a different thread outside the thread holding the JS runtime
- When the task is completed we use the `react::CallInvoker` object to interrupt safely the JS runtime and take over it to resolve or reject the `Promise`

Example:
```cpp
// function getReceiveAddress
return jsi::Function::createFromHostFunction(runtime, // create function using runtime
    jsi::PropNameID::forAscii(runtime, "getReceiveAddress"), // function name
    1, // number of arguments
    [this](jsi::Runtime& runtime, // lambda parameters
          const jsi::Value& thisValue,
          const jsi::Value* arguments,
          size_t count) -> jsi::Value { // lambda return type

        GA_json *details;
        utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details); // first argument is a JS json object that will be converted to a GA_json object

        // creates the function to be promisified
        utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){
            auto task = [=, &rt](){ // create the task that will be run in the separate thread
                // this part has no more access to the runtime and will be run in a separate thread
                try {
                    GA_auth_handler *call;
                    utils::wrapCall(GA_get_receive_address(session, details, &call));
                    json res = utils::resolve(call);
                    // the long running task finishes and we need to resolve or reject the promise
                    // lock the weak pointer to the invoker to gain control of the runtime
                    std::shared_ptr<react::CallInvoker> c = invoker.lock();
                    c->invokeAsync([=, &rt] { // invokes the runtime
                        // this part of the code is executed safely using the JS runtime
                        if (res.contains("result")) {
                            p->resolve(utils::parse(rt, res["result"].dump()));
                        } else {
                            p->reject(res["error"].dump());
                        }
                    });
                } catch (utils::Exception e) {
                    // same thing here to retun an error just in case something else fails
                    std::shared_ptr<react::CallInvoker> c = invoker.lock();
                    c->invokeAsync([=] { p->reject(e.what()); });
                }

            }; // end of the task body
            // queue the task in the thread pool that will be executed in a separate thread as soon as possible
            pool->queueWork(task);
        }; // end of the promise body
        // return immediately the promise to JS that will be resolved or rejected in the future
        // so that the runtime can continue its exection without blocking the UI
        return utils::makePromise(runtime, func);
});

```

## Debugging
- To debug the cpp code in iOS we can use the Xcode debugger, so run the example app from XCode
- To debug Android we can compile the example app with `yarn example android` and run it on the simulator, then type in a terminal `adb logcat` to see the native logs of the device

## Useful links
General documentation about JSI from official sources:
- https://reactnative.dev/docs/the-new-architecture/why#new-architectures-improvements
- https://reactnative.dev/docs/the-new-architecture/cxx-cxxturbomodules

A video that explains how to implement async tasks and Promises in a jsi project:
- https://www.youtube.com/watch?v=SC9PwcKw20o

In depth explanation of HostObjects and use cases:
- https://www.youtube.com/watch?v=_BNinSbzZTE

Best practices, project setup, general guidance:
- https://github.com/mrousavy/react-native-mmkv
- https://github.com/margelo/react-native-bignumber
