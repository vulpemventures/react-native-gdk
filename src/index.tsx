// eslint-disable-next-line @typescript-eslint/no-var-requires
const Gdk = require("./NativeGdk").default

export function multiply(a: number, b: number): number {
  // eslint-disable-next-line @typescript-eslint/no-unsafe-return
  return Gdk.multiply(a, b)
}
