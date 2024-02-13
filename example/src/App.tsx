/* eslint-disable no-console */
import * as React from "react"

import { StyleSheet, Text, SafeAreaView, Button } from "react-native"
import Gdk from "react-native-gdk"

const gdk = Gdk()


const App: React.FunctionComponent = () => {
  const [mnemonic] = React.useState(gdk.generateMnemonic12())
  const called = React.useRef(false)

  React.useEffect(() => {
    if (called.current) {
      return
    }

    gdk.init()
    gdk.createSession()

    gdk.addListener("network", (event) => {
      console.log("network event", event)
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
    }
  }, [])

  return (
    <SafeAreaView style={styles.container}>
      <Text>mnemonic: {mnemonic}</Text>
      <Button title="connect" onPress={() => {
        gdk.connect("electrum-testnet-liquid", "test-app")
      }}/>
      <Button title="register" onPress={() => {
        gdk.register({}, { mnemonic, password: "" })
      }}/>
      <Button title="login" onPress={() => {
        console.log(gdk.login({}, { mnemonic, password: "" }))
      }}/>
      <Button title="get subaccounts" onPress={() => {
        console.log(gdk.getSubaccounts({ refresh: false }))
      }}/>
      <Button title="create subaccount" onPress={async() => {
        try {
          console.log(await gdk.createSubaccount({ type: "p2wpkh", name: "SINGLE_SIG" }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="fee estimates" onPress={() => {
        console.log(gdk.getFeeEstimates())
      }}/>
      <Button title="receive address" onPress={() => {
        console.log(gdk.getReceiveAddress({ subaccount: 1 }))
      }}/>
      <Button title="validate mnemonic" onPress={() => {
        console.log(gdk.validateMnemonic("not valid mnemonic"))
        console.log(gdk.validateMnemonic(mnemonic))
      }}/>
      <Button title="get txs" onPress={() => {
        console.log(gdk.getTransactions({ subaccount: 1, first: 0, count: 10 }))
      }}/>
      <Button title="get utxos" onPress={() => {
        console.log(gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 }))
      }}/>
      <Button title="get prev addresses" onPress={() => {
        console.log(gdk.getPreviousAddresses({ subaccount: 1 }))
      }}/>
      <Button title="get mnemonic" onPress={() => {
        console.log(gdk.getMnemonic({ password: "" }))
      }}/>
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
