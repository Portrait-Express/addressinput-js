const { AddressValidator } = require("../dist/index.js");
const { expect } = require("expect");

describe("AddressValidator", () => {
    var cache = {};
    
    //Simplest viable implementation
    var validator = new AddressValidator({
        request: async (key) => {
            return await fetch("https://chromium-i18n.appspot.com/ssl-address/" + key).then(v => v.text());
        },
        get: async (key) => {
            return cache[key];
        },
        put: (key, val) => {
            cache[key] = val;
        }
    });

    it("should validate", async () => {
        let valid = await validator.validate({
            region_code: 'US',
            address_line: ['441 n water st'],
            administrative_area: 'OR',
            locality: 'Silverton',
            postal_code: "85192",
            organization: "Portrait Express",
        });
        expect(valid[1]).toEqual({POSTAL_CODE: ['MISMATCHING_VALUE']});
    });

    it("should format", async() => {
        let data = {
            region_code: 'US',
            address_line: ['441 n water st'],
            administrative_area: 'OR',
            locality: 'Silverton',
            postal_code: "97381",
            organization: "Portrait Express",
        } 

        let str = validator.format(data);
        
        expect(str).toEqual("Portrait Express\n441 n water st\nSilverton, OR 97381");
    })
});
