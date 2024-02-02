import * as React from "react"

import { StyleSheet, View, Text } from "react-native"
import { multiply } from "react-native-gdk"

const result = multiply(3, 7)

const App: React.FunctionComponent = () => {
  return (
    <View style={styles.container}>
      <Text>Result: {result}</Text>
    </View>
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
