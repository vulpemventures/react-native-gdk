const Gdk = require('./NativeGdk').default;

export function multiply(a: number, b: number): number {
  return Gdk.multiply(a, b);
}
