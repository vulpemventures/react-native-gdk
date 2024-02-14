import { createGdk, GdkInterface } from "./createGdk"

export default (): GdkInterface => {
  return createGdk()
}

export {
  GdkInterface
}
export type * from "./types"
