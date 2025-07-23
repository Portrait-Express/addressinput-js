//@ts-ignore
const addon = require("../lib/addressinput-js.node");

export type GetCallback = (key: string) => Promise<string>|string;
export type PutCallback = (key: string, data: string) => void;

/**
 * Options specified when creating a validator
 */
export type AddressValidatorOpts = {
    /**
     * Callback that requests the data from whatever source is expected
     */
    request: GetCallback,

    /**
     * Callback that gets any data that was previously stored with `put`
     */
    get: GetCallback,

    /**
     * Callback that stores a key's data to cache it for later
     */
    put: PutCallback
};

/**
 * Represents an issue with an address field.
 */
export enum AddressProblem {
    UNEXPECTED_FIELD = 'UNEXPECTED_FIELD',
    MISSING_REQUIRED_FIELD = 'MISSING_REQUIRED_FIELD',
    UNKNOWN_VALUE = 'UNKNOWN_VALUE',
    INVALID_FORMAT = 'INVALID_FORMAT',
    MISMATCHING_VALUE = 'MISMATCHING_VALUE',
    USES_P_O_BOX = 'USES_P_O_BOX',
    UNSUPPORTED_FIELD = 'UNSUPPORTED_FIELD',
};

/**
 * The address's full data object
 */
export type AddressData = {
    /**
     * The country of the address. Always expected, and should be the ISO 3166-2 character code.
     */
    region_code: string;

    /**
     * The street address lines of the address
     */
    address_line: string[];

    /**
     * The administrative area, state or province in the country. Likely expects abbreviation 
     * as well. Check data source.
     */
    administrative_area: string;

    /**
     * The address's full data object
     */
    locality: string;

    /**
     * The locality, or city for the address
     */
    dependent_locality: string;

    /**
     * Zip/Postal code
     */
    postal_code: string;

    /**
     * Additional sorting code
     */
    sorting_code: string;

    /**
     * Language for the address
     */
    language_code: string;

    /**
     * Organization the address represents
     */
    organization: string;

    /**
     * The recipient name
     */
    recipient: string;
}

/**
 * A map of issues to each field for the address
 */
export type FieldProblemMap = {
  COUNTRY?: AddressProblem[],
  ADMIN_AREA?: AddressProblem[],
  LOCALITY?: AddressProblem[],
  DEPENDENT_LOCALITY?: AddressProblem[],
  SORTING_CODE?: AddressProblem[],
  POSTAL_CODE?: AddressProblem[],
  STREET_ADDRESS?: AddressProblem[],
  ORGANIZATION?: AddressProblem[],
  RECIPIENT?: AddressProblem[]
}

export type ValidateAddressOpts = {
    allow_postal?: boolean,
    require_name?: boolean,
    filter?: FieldProblemMap
}

const defaultValidateAddressOpts: ValidateAddressOpts = {
    allow_postal: true,
    require_name: false,
    filter: {}
}

const defaultAddressData: AddressData = {
    region_code: "",
    address_line: [],
    administrative_area: "",
    locality: "",
    dependent_locality: "",
    postal_code: "",
    sorting_code: "",
    language_code: "",
    organization: "",
    recipient: "",
}

/**
 * Class to represent a validator instance.
 */
export class AddressValidator {
    private _validator: any;

    constructor(opts: AddressValidatorOpts) {
        this._validator = new addon.AddressValidator(opts);
    }

    /**
     * Validate an address object, and find issues
     *
     * @param {Partial<AddressData>} data The address object
     * @param {ValidateAddressOpts} [opts] Additional options for this validation
     * @returns {Promise<[AddressData, FieldProblemMap]>} Tuple of [validated address, problem map]
     */
    validate(data: Partial<AddressData>, opts?: ValidateAddressOpts): Promise<[AddressData, FieldProblemMap]> {
        return this._validator.validate(
            Object.assign({}, defaultAddressData, data), 
            Object.assign({}, defaultValidateAddressOpts, opts));
    }

    format(data: Partial<AddressData>) {
        return this._validator.format(Object.assign({}, defaultAddressData, data));
    }
}
