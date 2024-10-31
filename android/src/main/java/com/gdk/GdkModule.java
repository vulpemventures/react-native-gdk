package com.gdk;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.blockstream.libgreen_gdk.GDKJNI;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

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
      String rootDirectory = getReactApplicationContext().getFilesDir().getAbsolutePath() + "/gdk";
      CallInvokerHolderImpl jsCallInvokerHolder = (CallInvokerHolderImpl) getReactApplicationContext()
                                                  .getCatalystInstance()
                                                  .getJSCallInvokerHolder();

      installNativeJsi(jsContext.get(), jsCallInvokerHolder, rootDirectory);
      Log.i(NAME, "Successfully installed Gdk JSI Bindings!");
      return true;
    } catch (Exception exception) {
      Log.e(NAME, "Failed to install Gdk JSI Bindings!", exception);
      return false;
    }
  }


  private native void installNativeJsi(long jsiPtr, CallInvokerHolderImpl jsCallInvokerHolder, String datadir);

}
