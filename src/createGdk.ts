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
  getNetworks: () => Promise<GDK.GetNetworksRes>
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
  register: (hw_device: object, details: GDK.Credentials | GDK.PinCredentials) => Promise<void>
  /**
   * Logs in a user to gdk
   * @param hw_device - leave empty
   * @param userAgent - credentials used to authenticate the user
   */
  login: (hw_device: object, details: GDK.Credentials | GDK.PinCredentials) => Promise<void>
  /**
   * Lists the user's subaccounts
   * @param details - subaccount details containing `refresh` If set to true, subaccounts are re-discovered if appropriate for the session type. Note that this will take significantly more time if set
   */
  getSubaccounts: (details: { refresh: boolean }) => Promise<{ subaccounts: GDK.Subaccount[] }>
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
  }) => Promise<GDK.ReceiveAddressType>
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
   * @returns a list of transactions
   */
  getTransactions: (details: GDK.GetTransactionsReq) => Promise<{ transactions: GDK.Transaction[] }>
  /**
   * Get's the unspent outputs for a subaccount
   * @param details
   * @returns unspent outputs mapped by relative transaction
   */
  getUnspentOutputs: (details: GDK.GetSubaccountReq) => Promise<{ unspent_outputs: GDK.GetUnspentOutputsRes }>
  /**
   * Get's the fee estimates list for each next block
   * @returns a list of fee estimates
   */
  getFeeEstimates: () => Promise<{ fees: number[] }>
  /**
   * Get's the previous addresses for a subaccount with all their informations
   * @param details
   * @returns a list of previous addresses
   */
  getPreviousAddresses: (details: { subaccount: number }) => Promise<GDK.GetPreviousAddressesRes>
  /**
   * Get's the mnemonic using the password
   * @param details
   * @returns the mnemonic
   */
  getMnemonic: (details: { password: string }) => Promise<GDK.GetMenmonicReturnType>
  /**
   * Set's a pin for the user
   * @param details
   * @returns the pin data that has to be encrypted and stored on the device
   */
  setPin: (details: { pin: string, plaintext: GDK.Credentials }) => Promise<{ pin_data: GDK.PinData }>
  /**
   * Get's the transaction details for a transaction hash, these informations are the one that are not associated with the user's wallet, for example the hex of a transaction
   * @param txHash
   * @returns the transaction details
   */
  getTransactionDetails: (txHash: string) => Promise<GDK.TransactionDetails>
  /**
   * Create a transaction, passing `is_partial` = true will create a partial transaction (psbt)
   * @param details
   * @returns an unsigned transaction
   */
  createTransaction: (details: GDK.CreateTransactionReq) => Promise<GDK.UnsignedTransaction>
  /**
   * Blind a transaction
   * @param details - the unsigned transaction
   * @returns a blinded transaction
   */
  blindTransaction: (details: GDK.UnsignedTransaction) => Promise<GDK.BlindedTransaction>
  /**
   * Sign a transaction
   * @param details - an unsigned blinded transaction or a regular unsigned transaction
   * @returns a signed transaction
   */
  signTransaction: (details: GDK.BlindedTransaction | GDK.UnsignedTransaction) => Promise<GDK.SignedBlindedTransaction | GDK.SignedTransaction>
  /**
   * Send a transaction
   * @param details - a signed blinded transaction or a signed transaction
   * @returns the transaction object containing the txhash
   */
  sendTransaction: (details: GDK.SignedBlindedTransaction | GDK.SignedTransaction) => Promise<GDK.SignedBlindedTransaction | GDK.SignedTransaction>
  /**
   * broadcasts a transaction given his hexidecimal representation
   * @param txHex - the hex of the transaction
   * @returns the transaction hash
   */
  broadcastTransaction: (txHex: string) => Promise<string>
  signPsbt: (details: GDK.PsbtSignDetails) => Promise<{ psbt: string }>
  getBalance: (details: GDK.GetSubaccountReq) => Promise<{ [assetId: string]: number }>
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
    getReceiveAddress: gdk.getReceiveAddress,
    addListener: gdk.addListener,
    removeListener: gdk.removeListener,
    validateMnemonic: gdk.validateMnemonic,
    getTransactions: gdk.getTransactions,
    getUnspentOutputs: gdk.getUnspentOutputs,
    getFeeEstimates: gdk.getFeeEstimates,
    getPreviousAddresses: gdk.getPreviousAddresses,
    getMnemonic: gdk.getMnemonic,
    setPin: gdk.setPin,
    getTransactionDetails: gdk.getTransactionDetails,
    createTransaction: gdk.createTransaction,
    blindTransaction: gdk.blindTransaction,
    signTransaction: gdk.signTransaction,
    sendTransaction: gdk.sendTransaction,
    broadcastTransaction: gdk.broadcastTransaction,
    getNetworks: gdk.getNetworks,
    signPsbt: gdk.signPsbt,
    getBalance: gdk.getBalance
  }
}
