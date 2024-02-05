import { NativeModules, Platform } from "react-native"
import { GdkNativeInterface } from "./GdkNativeInterface"


export interface GdkInterface {
  /**
   * generates a new bip39 mnemonic with twelve words
   */
  generateMnemonic12: () => string
}

declare global {
  function gdkCreateNewInstance(): GdkNativeInterface
}

export const createGdk = (): GdkInterface => {
  // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
  if (!global.gdkCreateNewInstance) {
    const NativeGdk = NativeModules.NativeGdk

    if (!NativeGdk) {
      let message =
        "Failed to create a new Gdk instance: The native Gdk Module could not be found."
      message +=
        "\n* Make sure react-native-gdk is correctly autolinked (run `npx react-native config` to verify)"
      if (Platform.OS === "ios" || Platform.OS === "macos") {
        message += "\n* Make sure you ran `pod install` in the ios/ directory."
      }
      if (Platform.OS === "android") {
        message += "\n* Make sure gradle is synced."
      }
      throw new Error(message)
    }
    NativeGdk.install()
  }

  const gdk: GdkInterface = global.gdkCreateNewInstance()
  return gdk
}
