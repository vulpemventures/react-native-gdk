package com.gdk;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = GdkModule.NAME)
public class GdkModule extends NativeGdkSpec {
  public static final String NAME = "Gdk";

  public GdkModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @Override
  @NonNull
  public String getName() {
    return NAME;
  }

  static {
    System.loadLibrary("react-native-gdk");
  }

  private static native double nativeMultiply(double a, double b);

  // Example method
  // See https://reactnative.dev/docs/native-modules-android
  @Override
  public double multiply(double a, double b) {
    return nativeMultiply(a, b);
  }
}
