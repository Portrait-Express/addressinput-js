export type GetCallback = (key: string) => Promise<string> | string;
export type PutCallback = (key: string, data: string) => void;
export type AddressValidatorOpts = {
    request: GetCallback;
    get: GetCallback;
    put: PutCallback;
};
export declare enum AddressProblem {
    UNEXPECTED_FIELD = "UNEXPECTED_FIELD",
    MISSING_REQUIRED_FIELD = "MISSING_REQUIRED_FIELD",
    UNKNOWN_VALUE = "UNKNOWN_VALUE",
    INVALID_FORMAT = "INVALID_FORMAT",
    MISMATCHING_VALUE = "MISMATCHING_VALUE",
    USES_P_O_BOX = "USES_P_O_BOX",
    UNSUPPORTED_FIELD = "UNSUPPORTED_FIELD"
}
export type AddressData = {
    region_code: string;
    address_line: string[];
    administrative_area: string;
    locality: string;
    dependent_locality: string;
    postal_code: string;
    sorting_code: string;
    language_code: string;
    organization: string;
    recipient: string;
};
export type FieldProblemMap = {
    COUNTRY?: AddressProblem[];
    ADMIN_AREA?: AddressProblem[];
    LOCALITY?: AddressProblem[];
    DEPENDENT_LOCALITY?: AddressProblem[];
    SORTING_CODE?: AddressProblem[];
    POSTAL_CODE?: AddressProblem[];
    STREET_ADDRESS?: AddressProblem[];
    ORGANIZATION?: AddressProblem[];
    RECIPIENT?: AddressProblem[];
};
export type ValidateAddressOpts = {
    allow_postal?: boolean;
    require_name?: boolean;
    filter?: FieldProblemMap;
};
export declare class AddressValidator {
    private _validator;
    constructor(opts: AddressValidatorOpts);
    validate(data: Partial<AddressData>, opts?: ValidateAddressOpts): Promise<[AddressData, FieldProblemMap]>;
}
