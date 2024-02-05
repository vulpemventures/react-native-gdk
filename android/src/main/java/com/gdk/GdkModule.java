package com.gdk;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = GdkModule.NAME)
public class GdkModule extends ReactContextBaseJavaModule {
  public static final String NAME = "NativeGdk";

  public GdkModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @NonNull
  @Override
  public String getName() {
    return NAME;
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      Log.i(NAME, "Loading C++ library...");
      System.loadLibrary("react-native-gdk");

      JavaScriptContextHolder jsContext = getReactApplicationContext().getJavaScriptContextHolder();
      // if (rootDirectory == null) {
      //   rootDirectory = getReactApplicationContext().getFilesDir().getAbsolutePath() + "";
      // }
      // Log.i(NAME, "Installing Gdk JSI Bindings fo root directory: " + rootDirectory);
      nativeInstall(jsContext.get());
      Log.i(NAME, "Successfully installed Gdk JSI Bindings!");
      return true;
    } catch (Exception exception) {
      Log.e(NAME, "Failed to install Gdk JSI Bindings!", exception);
      return false;
    }
  }

  private static native void nativeInstall(long jsiPtr);
}
