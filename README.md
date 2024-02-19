# react-native-gdk
React Native integration of [Green Development Kit](https://github.com/Blockstream/gdk) for Liquid Bitcoin.
* Made entirely using JSI (JavaScript Interface) for React Native.
* Currently using GDK [Release 0.69.0](https://github.com/Blockstream/gdk/releases/tag/release_0.69.0)

## Installation
<h3>
  React Native
</h3>

```bash
npm install @vulpemventures/react-native-gdk
# or
yarn add @vulpemventures/react-native-gdk
# and then
cd ios && pod install
```
<h3>
  Expo
</h3>

```bash
yarn add @vulpemventures/react-native-gdk
# and then
expo prebuild
```

## Usage


```js
import Gdk from "react-native-gdk"
// create the instance of your native module
const gdk = Gdk()
// make sure to call
gdk.init()
gdk.createSession()

// you are good to go!
gdk.generateMnemonic12()
```

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.
