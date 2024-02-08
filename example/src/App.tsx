/* eslint-disable no-console */
import * as React from "react"

import { StyleSheet, Text, SafeAreaView, Button } from "react-native"
import Gdk from "react-native-gdk"

const gdk = Gdk()


const App: React.FunctionComponent = () => {
  const [mnemonic, setMnemonic] = React.useState(gdk.generateMnemonic12())
  const called = React.useRef(false)

  React.useEffect(() => {
    if (called.current) {
      return
    }

    gdk.init()
    gdk.createSession()

    gdk.on("network", (event) => {
      console.log("network event", event)
    })

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
      <Button title="create subaccount" onPress={() => {
        console.log(gdk.createSubaccount({ type: "p2wpkh", name: "SINGLE_SIG" }))
      }}/>
      <Button title="receive address" onPress={() => {
        console.log(gdk.getReceiveAddress({ subaccount: 1 }))
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
