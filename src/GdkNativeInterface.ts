// this represents the native module type

export type Network = "electrum-testnet-liquid" | "electrum-liduid"
export type Credentials = {
  mnemonic: string
  password: string
}

export interface GdkNativeInterface {
  generateMnemonic12: () => string
  init: (log_level: "debug" | "none") => void
  createSession: () => void
  connect: (name: Network, userAgent: string) => void
  register: (hw_device: object, details: Credentials) => void
  login: (hw_device: object, details: Credentials) => void
  getSubaccounts: (details: { refresh: boolean }) => object
}
