// this represents the native module type

export type Network = "electrum-testnet-liquid" | "electrum-liduid"
export type Credentials = {
  mnemonic: string
  password: string
}
export type SubaccountType = "p2sh-p2wpkh" | "p2wpkh" | "p2pkh" | "2of2_no_recovery" | "2of3"
export type CreateSubaccountDetails = {
  name: string
  type: SubaccountType
}

export type Events = {
  network: {
    event: "network"
    network: {
      wait_ms: number
      current_state: "connected" | "disconnected"
      next_state: "connected" | "disconnected"
    }
  }
  block: {
    event: "block"
    block: {
      block_hash: string
      number: number
      initial_timestamp: number
      previous_hash: string
    }
  }
}


export type EventType = keyof Events
export type NotificationHandler = <T extends EventType>(event: T, fn: (eventData: Events[T]) => void) => void

export type Subaccount = {
  bip44_discovered: false
  hidden: false
  name: ""
  pointer: 0
  receiving_id: ""
  required_ca: 0
  type: SubaccountType
}

export type ReceiveAddressType = {
  address: string
  address_type: "csv" | "p2sh" | "p2wsh" | "p2pkh" | "p2sh-p2wpkh" | "p2wpkh"
  blinding_key: string
  is_confidential: boolean
  is_internal: boolean
  pointer: boolean
  scriptpubkey: string
  subaccount: number
  unconfidential_address: string
  user_path: number[]
}

export interface GdkNativeInterface {
  generateMnemonic12: () => string
  init: (log_level: "debug" | "none") => void
  createSession: () => void
  connect: (name: Network, userAgent: string) => void
  register: (hw_device: object, details: Credentials) => void
  login: (hw_device: object, details: Credentials) => void
  getSubaccounts: (details: { refresh: boolean }) => {
    subaccounts: Subaccount[]
  }
  createSubaccount: (details: CreateSubaccountDetails) => {
    bip44_discovered: boolean
    core_descriptors: string[]
    hidden: boolean
    name: string
    pointer: number
    receiving_id: string
    required_ca: number
    type: SubaccountType
    user_path: number[]
  }
  getReceiveAddress: (details: {
    subaccount: number
    is_internal: boolean
    ignore_gap_limit: boolean
  }) => ReceiveAddressType
  on: NotificationHandler
}
