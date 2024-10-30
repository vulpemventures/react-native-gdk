# How to update GDK version
To update GDK version there are a couple things that are needed.

1. To get started check [CONTRIBUTING.md](/CONTRIBUTING.md)
2. Change the release name in the [`android_gdk`](/npm_scripts/android_gdk.sh) and the [`ios_gdk`](/npm_scripts/ios_gdk.sh)
3. Update SHA256 values for each downloaded release
4. Run `yarn prepack` to update gdk prebuilts
5. Add whatever new methods are needed in the [`GdkHostObject.cpp`](/cpp/GdkHostObject.cpp) file
