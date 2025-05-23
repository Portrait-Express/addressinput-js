import { createRequire } from "node:module"
const require = createRequire(import.meta.url);

const addon = require("./lib/addressinput-js.node");

export const AddressValidator = addon.AddressValidator;
export default addon;
