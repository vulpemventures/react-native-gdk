/* eslint-disable no-console */
import * as React from "react"
import {
  StyleSheet,
  Text,
  SafeAreaView,
  Button,
  ScrollView
} from "react-native"
import Gdk from "@vulpemventures/react-native-gdk"
import { Platform } from "react-native"

const NIGIRI_IP = Platform.OS === "android" ? "10.0.2.2" : "localhost"

const gdk = Gdk()

const App: React.FunctionComponent = () => {
  const [mnemonic, setMnemonic] = React.useState(gdk.generateMnemonic12())
  const [connected, setConnected] = React.useState(false)
  const called = React.useRef(false)

  React.useEffect(() => {
    if (called.current) {
      return
    }

    gdk.init()
    gdk.createSession()

    gdk.addListener("network", (event) => {
      console.log("network event", event)
      setConnected(true)
    })

    gdk.addListener("transaction", (event) => {
      console.log("transaction event", event)
    })

    gdk.addListener("subaccount", (event) => {
      console.log("SUBACCOUNT event", event)
    })

    return () => {
      gdk.removeListener("network")
      gdk.removeListener("transaction")
      gdk.removeListener("subaccount")
    }
  }, [])

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView>
        <Text>mnemonic: {mnemonic}</Text>
        <Text>Connected: {connected ? "yes" : "no"}</Text>
        <Button title="createSession" onPress={() => {
          gdk.createSession()
        }} />
        <Button title="gen mnemonic" onPress={() => {
          setMnemonic(gdk.generateMnemonic12())
        }} />
        <Button title="connect" onPress={() => {
          gdk.connect("electrum-testnet-liquid", "test-app")
        }} />
        <Button title="connect to localtest" onPress={() => {
          gdk.connect("electrum-localtest-liquid", "test-app")
        }} />
        <Button title="register" onPress={async() => {
          try {
            console.log(await gdk.register({}, { mnemonic, password: "" }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="destroy session" onPress={async() => {
          try {
            console.log(gdk.destroySession())
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="login" onPress={async() => {
          try {
            console.log(await gdk.login({}, { mnemonic, password: "" }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="set pin" onPress={async() => {
          try {
            console.log(await gdk.setPin({ pin: "123456", plaintext: { mnemonic, password: "" } }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get subaccounts" onPress={async() => {
          try {
            console.log(await gdk.getSubaccounts({ refresh: false }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="create subaccount" onPress={async() => {
          try {
            console.log(await gdk.createSubaccount({ type: "p2wpkh", name: "SINGLE_SIG" }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="fee estimates" onPress={async() => {
          try {
            console.log(await gdk.getFeeEstimates())
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="receive address" onPress={async() => {
          try {
            console.log(await gdk.getReceiveAddress({ subaccount: 1 }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="validate mnemonic" onPress={() => {
          console.log(gdk.validateMnemonic("not valid mnemonic"))
          console.log(gdk.validateMnemonic(mnemonic))
        }} />
        <Button title="get txs" onPress={async() => {
          try {
            console.log(await gdk.getTransactions({ subaccount: 1, first: 0, count: 10 }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get utxos" onPress={async() => {
          try {
            console.log(await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get balance" onPress={async() => {
          try {
            console.log(await gdk.getBalance({ subaccount: 1, num_confs: 0 }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get prev addresses" onPress={async() => {
          try {
            console.log(await gdk.getPreviousAddresses({ subaccount: 1 }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get mnemonic" onPress={async() => {
          try {
            console.log(await gdk.getMnemonic({ password: "" }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="get tx details" onPress={async() => {
          try {
            console.log(await gdk.getTransactionDetails("6a8fe6a79635fbdaddd3ec14c0e11325b41f46e349e19a89af0d6e2d89d47e32"))
          } catch (error) {
            console.log("ERROR", error)
          }
          try {
            console.log(await gdk.getTransactionDetails("c72b18cf226549af2befd1b9941a8b9a0d91d2a0e65df8f7e8e3d8af336a89bd"))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="test create transaction" onPress={async() => {
          try {
            console.log(await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            }))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="blind transaction" onPress={async() => {
          try {
            const unsignedTx = await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            })

            console.log(await gdk.blindTransaction(unsignedTx))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="sign transaction unblinded" onPress={async() => {
          try {
            const unsignedTx = await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            })

            console.log(await gdk.signTransaction(unsignedTx))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="sign transaction blinded" onPress={async() => {
          try {
            const unsignedTx = await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            })
            const blinded = await gdk.blindTransaction(unsignedTx)
            console.log(await gdk.signTransaction(blinded))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="send transaction unblinded" onPress={async() => {
          try {
            const unsignedTx = await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            })
            const signed = await gdk.signTransaction(unsignedTx)
            console.log(await gdk.broadcastTransaction(signed.transaction))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
        <Button title="send transaction blinded" onPress={async() => {
          try {
            const unsignedTx = await gdk.createTransaction({
              addressees: [
                {
                  address: "tlq1qqtrn8tvc22hdtyxrcqq9dugr6wrq45fpgxnjzh8lpttx4tc2uq4xtveedmnq3z40mlr6fjj2q704ert4ekqqjnua8gw9068wu",
                  asset_id: "144c654344aa716d6f3abcc1ca90e5641e4e2a7f633bc09fe3baf64585819a49",
                  satoshi: 1000
                }
              ],
              utxos: (await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 })).unspent_outputs
            })
            const blinded = await gdk.blindTransaction(unsignedTx)
            const signed = await gdk.signTransaction(blinded)
            console.log(await gdk.sendTransaction(signed))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />

        <Button title="register localtest network" onPress={async() => {
          try {
            console.log(gdk.registerNetwork("electrum-localtest-liquid", {
              name: "Electrum Liquid Regtest",
              network: "electrum-localtest-liquid",
              address_explorer_url: `http://${NIGIRI_IP}:5001/address/`,
              electrum_url: `${NIGIRI_IP}:50001`,
              spv_enabled: false,
              tls: false,
              asset_registry_url: "https://assets.blockstream.info",
              asset_registry_onion_url: "http://vi5flmr4z3h3luup.onion",
              tx_explorer_url: `http://${NIGIRI_IP}:3001/tx/`,
              mainnet: false,
              liquid: true,
              development: true,
              policy_asset: "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225",
              ct_exponent: 0,
              ct_bits: 52,
              bip21_prefix: "liquidregtestnetwork",
              server_type: "electrum",
              spv_cross_validation: false,
              spv_cross_validation_servers: [],
              p2pkh_version: 235,
              p2sh_version: 75,
              pin_server_url: "https://jadepin.blockstream.com",
              pin_server_public_key: "0332b7b1348bde8ca4b46b9dcc30320e140ca26428160a27bdbfc30b34ec87c547",
              pin_server_onion_url: "http://mrrxtq6tjpbnbm7vh5jt6mpjctn7ggyfy5wegvbeff3x7jrznqawlmid.onion"
            }))

            const networks = await gdk.getNetworks()

            console.log(networks)

            console.log("is electrum-localtest-liquid registered?", networks.all_networks.includes("electrum-localtest-liquid"))
            console.log("is non-existent-network registered", networks.all_networks.includes("non-existent-network"))
          } catch (error) {
            console.log("ERROR", error)
          }
        }} />
      </ScrollView>
    </SafeAreaView>
  )
}

export default App

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: "center",
    justifyContent: "center"
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20
  }
})
