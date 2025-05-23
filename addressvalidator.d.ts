declare module './index.js' {
    type AddressValidatorOpts = {
        request: (key: string) => Promise<string>|string,
        get: (key: string) => Promise<string>|string,
        put: (key: string) => void
    };

    class AddressValidator {
        constructor(opts: AddressValidatorOpts);

        validate();
    }
}
