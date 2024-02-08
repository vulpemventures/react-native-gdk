// this represents the native module type
import type * as GDK from "./types"


export interface GdkNativeInterface {
  generateMnemonic12: () => string
  init: (log_level: "debug" | "none") => void
  createSession: () => void
  connect: (name: GDK.Network, userAgent: string) => void
  register: (hw_device: object, details: GDK.Credentials) => void
  login: (hw_device: object, details: GDK.Credentials) => void
  getSubaccounts: (details: { refresh: boolean }) => {
    subaccounts: GDK.Subaccount[]
  }
  createSubaccount: (details: GDK.CreateSubaccountDetails) => {
    bip44_discovered: boolean
    core_descriptors: string[]
    hidden: boolean
    name: string
    pointer: number
    receiving_id: string
    required_ca: number
    type: GDK.SubaccountType
    user_path: number[]
  }
  getReceiveAddress: (details: {
    subaccount: number
    is_internal: boolean
    ignore_gap_limit: boolean
  }) => GDK.ReceiveAddressType
  addListener: GDK.NotificationHandler
  removeListener: (evt: GDK.EventType) => void
  validateMnemonic: (mnemonic: string) => boolean
  getTransactions: (details: GDK.GetTransactionsReq) => { transactions: GDK.Transaction[] }
  getUnspentOutputs: (details: GDK.GetSubaccountReq) => { unspent_outputs: GDK.UnspentOutput[] }
  getFeeEstimates: () => { fees: number[] }
  getPreviousAddresses: (details: { subaccount: number }) => GDK.GetPreviousAddressesRes
  getMnemonic: (details: { password: string }) => GDK.GetMenmonicReturnType
}
