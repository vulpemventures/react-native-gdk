export type Network =
  "mainnet" |
  "liquid" |
  "testnet" |
  "testnet-liquid" |
  "electrum-liquid" |
  "electrum-mainnet" |
  "electrum-testnet" |
  "electrum-testnet-liquid"

export type Credentials = {
  mnemonic: string
  password: string
}

export type PinCredentials = {
  pin: string
  pin_data: {
    encrypted_data: string
    pin_identifier: string
    salt: string
  }
}

export type SubaccountType = "p2sh-p2wpkh" | "p2wpkh" | "p2pkh" | "2of2_no_recovery" | "2of3"
export type AddressType = "csv" | "p2sh" | "p2wsh" | "p2pkh" | "p2sh-p2wpkh" | "p2wpkh"
export type CreateSubaccountDetails = {
  name: string
  type: SubaccountType
}

export type GetNetworksRes = {
  all_networks: Network[]
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
  transaction: {
    event: "transaction"
    transaction: {
      subaccounts: number[]
      txhash: string
    }
  }
  subaccount: {
    event: "subaccount"
    subaccount: {
      pointer: number
      event_type: "new" | "synced"
    }
  }
}


export type EventType = keyof Events
export type NotificationHandler = <T extends EventType>(event: T, fn: (eventData: Events[T]) => void) => void

export type GetTransactionsReq = {
  subaccount: number
  first: number
  count: number
}

export type GetSubaccountReq = {
  subaccount: number
  num_confs: number
  // defaults: ""
  address_type?: AddressType | ""
  // defaults true
  all_coins?: boolean
  // defaults: false
  confidential?: boolean
  // no default
  dust_limit?: number
  sort_by?: "oldest" | "newest" | "largest" | "smallest"
}

export type CreateSubaccountReturnType = {
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
  address_type: AddressType
  blinding_key: string
  is_confidential: boolean
  is_internal: boolean
  pointer: boolean
  scriptpubkey: string
  subaccount: number
  unconfidential_address: string
  user_path: number[]
}

export type Input = {
  address: string
  address_type: "p2pkh" | "p2sh-p2wpkh" | "p2wpkh"
  is_internal: boolean
  is_relevant: boolean
  is_spent: boolean
  // For user wallet addresses, the address number/final number in the address derivation path.
  pointer: number
  satoshi: number
  script: string
  subaccount: number
  subtype: number
}

export type Output = {
  address: string
  address_type: "p2pkh" | "p2sh-p2wpkh" | "p2wpkh"
  amountblinder: string
  asset_id: string
  assetblinder: string
  blinding_key: string
  is_blinded: boolean
  is_confidential: boolean
  is_internal: boolean
  is_relevant: boolean
  is_spent: boolean
  // For user wallet addresses, the address number/final number in the address derivation path.
  pointer: number
  satoshi: number
  script: string
  subaccount: number
  subtype: number
  unconfidential_address: string
}

export type Transaction = {
  block_height: number
  can_cpfp: boolean
  can_rbf: boolean
  created_at_ts: number
  fee: number
  fee_rate: number
  inputs: Input[]
  memo: string
  outputs: Output[]
  rbf_optin: boolean
  // A map of asset names to the signed satoshi total for that asset in the transaction. Negative numbers represent outgoing amounts, positive incoming.
  satoshi: {
    [asset: string]: number
  }
  spv_verified: string
  transaction_vsize: number
  transaction_weight: number
  txhash: string
  type: "incoming" | "outgoing" | "mixed" | "not unblindable"
}

export type UnspentOutput = {
  address_type: AddressType
  amountblinder: string
  asset_id: string
  asset_tag: string
  assetblinder: string
  block_height: number
  commitment: string
  is_blinded: boolean
  is_confidential: boolean
  is_internal: boolean
  nonce_commitment: string
  // The user key number/final number in the derivation path.
  pointer: number
  prevout_script: string
  // The index of the output, the vout.
  pt_idx: number
  public_key: string
  satoshi: number
  subaccount: number
  txhash: string
  user_path: number[]
}

export type GetUnspentOutputsRes = {
  [asset: string]: UnspentOutput[]
}

export type Address = {
  address: string
  address_type: AddressType
  blinding_key: string
  is_confidential: boolean
  is_internal: boolean
  pointer: number
  script: string
  scriptpubkey: string
  subaccount: number
  tx_count: number
  unconfidential_address: string
  user_path: number[]
}

export type GetPreviousAddressesRes = {
  list: Address[]
}

export type GetMenmonicReturnType = {
  master_blinding_key: string
  mnemonic: string
  seed: string
}

export type PinData = {
  encrypted_data: string
  hmac: string
  pin_identifier: string
  salt: string
}

export type TransactionDetails = {
  transaction: string
  transaction_locktime: number
  transaction_version: number
  transaction_vsize: number
  transaction_weight: number
  txhash: string
}

export type Addressee = {
  address: string
  satoshi: number
  asset_id: string
}

export type CreateTransactionReq = {
  addressees: Addressee[]
  utxos: {
    [asset: string]: UnspentOutput[]
  }
  is_partial?: boolean
}

export type TxChangeOutput = {
  address: string
  address_type: AddressType
  asset_id: string
  blinding_key: string
  is_change: true
  is_confidential: boolean
  is_internal: boolean
  pointer: number
  satoshi: number
  scriptpubkey: string
  subaccount: number
  unconfidential_address: string
  user_path: number[]
}

export type TxOutput = {
  address: string
  asset_id: string
  blinding_key: string
  satoshi: number
  scriptpubkey: string
}

export type TxInput = {
  address_type: AddressType
  amountblinder: string
  asset_id: string
  asset_tag: string
  assetblinder: string
  block_height: number
  commitment: string
  is_blinded: boolean
  is_confidential: boolean
  is_internal: boolean
  nonce_commitment: boolean
  pointer: number
  prevout_script: string
  pt_idx: number
  public_key: string
  satoshi: number
  sequence: number
  subaccount: number
  txhash: string
  user_path: number[]
}

export type FeeOutput = {
  asset_id: string
  satoshi: number
  scriptpubkey: string
}

export type UnsignedTransaction = {
  addresses: Addressee[]
  calculated_fee_rate: number
  change_address: {
    [asset: string]: Address
  }
  change_amount: {
    [asset: string]: number
  }
  change_subaccount: number
  fee: number
  fee_rate: number
  network_fee: number
  satoshi: {
    [asset: string]: number
  }
  transaction: string
  transaction_inputs: TxInput[]
  transaction_locktime: number
  transaction_outputs: (TxOutput | TxChangeOutput | FeeOutput)[]
  transaction_version: number
  transaction_vsize: number
  transaction_weight: number
  utxo_strategy: string
  utxos: {
    [asset: string]: UnspentOutput[]
  }
}

export type BlindedTxOutput = TxOutput & {
  amountblinder: string
  assetblinder: string
  eph_public_key: string
}

export type BlindedChangeOutput = TxChangeOutput & {
  amountblinder: string
  assetblinder: string
  eph_public_key: string
}

export type BlindedTransaction = UnsignedTransaction & {
  transaction: string
  is_blinded: true
  transaction_outputs: (BlindedTxOutput | FeeOutput | BlindedChangeOutput)[]
}

export type SignedTransaction = UnsignedTransaction & {
  txhash: string
}


export type SignedBlindedTransaction = BlindedTransaction & {
  txhash: string
}

export type PsbtSignDetails = {
  psbt: string
  // eslint-disable-next-line @typescript-eslint/ban-types
  utxos: unknown[]
  // eslint-disable-next-line @typescript-eslint/ban-types
  blinding_nonces?: unknown[]
}
