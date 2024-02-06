import { NativeModules, Platform } from "react-native"
import * as GDK from "./GdkNativeInterface"


export interface GdkInterface {
  /**
   * generates a new bip39 mnemonic with twelve words
   */
  generateMnemonic12: () => string
  /**
   * initializes gdk
   */
  init: () => void
  /**
   * creates a new session
   */
  createSession: () => void
  /**
   * connects to the network
   * @param name - network name
   * @param userAgent - user agent
   */
  connect: (name: GDK.Network, userAgent: string) => void
  register: (hw_device: object, details: GDK.Credentials) => void
}

declare global {
  function gdkCreateNewInstance(): GDK.GdkNativeInterface
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

  const gdk = global.gdkCreateNewInstance() as GDK.GdkNativeInterface
  return {
    createSession: gdk.createSession,
    generateMnemonic12: gdk.generateMnemonic12,
    init: (): void => {
      try {
        gdk.init(__DEV__ ? "debug" : "none")
      } catch (error) {
        // eslint-disable-next-line @typescript-eslint/ban-types
        if (!(__DEV__ && (module as unknown as { hot: object | undefined }).hot)) {
          throw error
        }
      }
    },
    connect: gdk.connect,
    register: gdk.register
  }
}
