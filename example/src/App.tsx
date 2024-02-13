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
      gdk.removeListener("subaccount")
    }
  }, [])

  return (
    <SafeAreaView style={styles.container}>
      <Text>mnemonic: {mnemonic}</Text>
      <Button title="connect" onPress={() => {
        gdk.connect("electrum-testnet-liquid", "test-app")
      }}/>
      <Button title="register" onPress={async() => {
        try {
          await gdk.register({}, { mnemonic, password: "" })
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="login" onPress={async() => {
        try {
          console.log(await gdk.login({}, { mnemonic, password: "" }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="set pin" onPress={async() => {
        try {
          console.log(await gdk.setPin({ pin: "123456", plaintext: { mnemonic, password: "" } }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="get subaccounts" onPress={async() => {
        try {
          console.log(await gdk.getSubaccounts({ refresh: false }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="create subaccount" onPress={async() => {
        try {
          console.log(await gdk.createSubaccount({ type: "p2wpkh", name: "SINGLE_SIG" }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="fee estimates" onPress={async() => {
        try {
          console.log(await gdk.getFeeEstimates())
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="receive address" onPress={async() => {
        try {
          console.log(await gdk.getReceiveAddress({ subaccount: 1 }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="validate mnemonic" onPress={() => {
        console.log(gdk.validateMnemonic("not valid mnemonic"))
        console.log(gdk.validateMnemonic(mnemonic))
      }}/>
      <Button title="get txs" onPress={async() => {
        try {
          console.log(await gdk.getTransactions({ subaccount: 1, first: 0, count: 10 }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="get utxos" onPress={async() => {
        try {
          console.log(await gdk.getUnspentOutputs({ subaccount: 1, num_confs: 0 }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="get prev addresses" onPress={async() => {
        try {
          console.log(await gdk.getPreviousAddresses({ subaccount: 1 }))
        } catch (error) {
          console.log("ERROR", error)
        }
      }}/>
      <Button title="get mnemonic" onPress={async() => {
        try {
          console.log(await gdk.getMnemonic({ password: "" }))
        } catch (error) {
          comsole.log("ERROR", error)
        }
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
