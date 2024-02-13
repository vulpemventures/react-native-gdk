import { NativeModules, Platform } from "react-native"
import * as GDK from "./types"
import { GdkNativeInterface } from "./GdkNativeInterface"


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
   * creates a new session with gdk, this should be called before any other gdk method
   */
  createSession: () => void
  /**
   * connects to the network
   * This called right after init and before any other gdk method
   * @param name - network name
   * @param userAgent - user agent
   */
  connect: (name: GDK.Network, userAgent: string) => void
  /**
   * Registers a new user to gdk calling this method more than once will not cause an error
   * @param hw_device - leave empty
   * @param userAgent - credentials used to authenticate the user
   */
  register: (hw_device: object, details: GDK.Credentials) => void
  /**
   * Logs in a user to gdk
   * @param hw_device - leave empty
   * @param userAgent - credentials used to authenticate the user
   */
  login: (hw_device: object, details: GDK.Credentials) => void
  /**
   * Lists the user's subaccounts
   * @param details - subaccount details containing `refresh` If set to true, subaccounts are re-discovered if appropriate for the session type. Note that this will take significantly more time if set
   */
  getSubaccounts: (details: { refresh: boolean }) => object
  /**
   * Creates a new subaccount
   * @param details - subaccount details: name (unique per wallet) and type
   */
  createSubaccount: (details: GDK.CreateSubaccountDetails) => Promise<GDK.CreateSubaccountReturnType>
  /**
   * Get's the receive address for a subaccount
   * @param details - details: `subaccount` pointer number, `is_internal` will retrieve an address that can be used exclusevely to receive changes from txs defaults false, `ignore_gap_limit` Whether to allow squentially generated addresses to go beyond the "gap_limit" passed to or defaulted by GA_connect. This is potentially dangerous as funds received on such addresses are not synced until an address within the gap_limit receives funds defaults false.
   */
  getReceiveAddress: (details: {
    subaccount: number
    is_internal?: boolean
    ignore_gap_limit?: boolean
  }) => GDK.ReceiveAddressType
  /**
   * Adds or overwrites a notification handler
   */
  addListener: GDK.NotificationHandler
  /**
   * Removes a notification handler
   */
  removeListener: (evt: GDK.EventType) => void
  /**
   * Validates a mnemonic
   * @param mnemonic - the mnemonic to validate
   */
  validateMnemonic: (mnemonic: string) => boolean
  /**
   *  Get's the transactions for a subaccount
   * @param details - subaccount number and listing parameters
   * @returns - a list of transactions
   */
  getTransactions: (details: GDK.GetTransactionsReq) => { transactions: GDK.Transaction[] }
  /**
   * Get's the unspent outputs for a subaccount
   * @param details
   * @returns - unspent outputs mapped by relative transaction
   */
  getUnspentOutputs: (details: GDK.GetSubaccountReq) => { unspent_outputs: GDK.UnspentOutput[] }
  getFeeEstimates: () => { fees: number[] }
  getPreviousAddresses: (details: { subaccount: number }) => GDK.GetPreviousAddressesRes
  getMnemonic: (details: { password: string }) => GDK.GetMenmonicReturnType
}

declare global {
  // eslint-disable-next-line no-var
  var GDK: GdkNativeInterface
}

export const createGdk = (): GdkInterface => {
  // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
  if (!global.GDK) {
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


  const gdk = global.GDK

  return {
    createSession: (): void => {
      try {
        gdk.createSession()
      } catch (error) {
        // eslint-disable-next-line @typescript-eslint/ban-types
        if (!(__DEV__ && (module as unknown as { hot: object | undefined }).hot)) {
          throw error
        }
      }
    },
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
    register: gdk.register,
    login: gdk.login,
    getSubaccounts: gdk.getSubaccounts,
    createSubaccount: gdk.createSubaccount,
    getReceiveAddress: ({
      ignore_gap_limit = false,
      is_internal = false,
      subaccount
    }): GDK.ReceiveAddressType => {
      return gdk.getReceiveAddress({ ignore_gap_limit, is_internal, subaccount })
    },
    addListener: gdk.addListener,
    removeListener: gdk.removeListener,
    validateMnemonic: gdk.validateMnemonic,
    getTransactions: gdk.getTransactions,
    getUnspentOutputs: gdk.getUnspentOutputs,
    getFeeEstimates: gdk.getFeeEstimates,
    getPreviousAddresses: gdk.getPreviousAddresses,
    getMnemonic: gdk.getMnemonic
  }
}
