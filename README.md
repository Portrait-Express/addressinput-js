# addressinput-js
The purpose of this library is to provide address validation through [google/libaddressinput](https://github.com/google/libaddressinput) in NodeJS through a NodeJS CPP addon.

## Installation
```bash
npm i addressinput-js
```

## Usage
```js
import { AddressValidator } from "addressinput-js"

var cache = {};
var validator = new AddressValidator({
  request: (key) => fetch("https://chromium-i18n.appspot.com/ssl-address/" + key).then(v => v.text()),
  get: (key) => cache[key],
  put:  (key, val) => cache[key] = val
});
await validator.validate({
  region_code: 'US',
  address_line: ['441 n water st'],
  administrative_area: 'OR',
  locality: 'Silverton',
  postal_code: "85192",
  organization: "Portrait Express"
}); // { POSTAL_CODE: [ 'MISMATCHING_VALUE' ] }
```

The only function exposed through `AddressValidator` currently is `validate`. More may be added later.

## Building From Source
```bash
git clone https://github.com/Portrait-Express/addressinput-js
cd addressinput-js
git submodule update --recursive
npm run build
```
The above command builds the cpp addon and cpoies the binary to `lib/addressinput-js.node`, then runs `tsc` to build `dist/index.js`. To only build the `.node` binary run `npm run build:cpp` instead of `build`.

## Todo
The plan is to become more of a direct wrapper around libaddressinput with dedicated Source and Storage objects. Also need a method to get data for an arbitrary key.
