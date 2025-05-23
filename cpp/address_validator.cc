#include <cstdio>
#include <sstream>
#include <vector>

#include <libaddressinput/address_data.h>

#include "address_validator.h"

std::string get_napi_type_name(Napi::Env env, napi_valuetype type) {
    switch(type) {
        case napi_valuetype::napi_bigint:
            return "bigint";
        case napi_valuetype::napi_boolean:
            return "boolean";
        case napi_valuetype::napi_external:
            return "external";
        case napi_valuetype::napi_function:
            return "napi_function";
        case napi_valuetype::napi_null:
            return "null";
        case napi_valuetype::napi_number:
            return "number";
        case napi_valuetype::napi_object:
            return "object";
        case napi_valuetype::napi_string:
            return "string";
        case napi_valuetype::napi_symbol:
            return "symbol";
        case napi_valuetype::napi_undefined:
            return "undefined";
        default:
            throw unexpected_type_exception(env, "Unknown value for napi_valuetype");
    }
}

Napi::Error unexpected_type_exception(
        Napi::Env env,
        std::string propName, 
        napi_valuetype expected, 
        napi_valuetype received) noexcept {
    return unexpected_type_exception(env, propName, get_napi_type_name(env, expected), get_napi_type_name(env, received));
}

Napi::Error unexpected_type_exception(
        Napi::Env env,
        std::string propName, 
        std::string expected, 
        napi_valuetype received) noexcept {
    return unexpected_type_exception(env, propName, expected, get_napi_type_name(env, received));
}

Napi::Error unexpected_type_exception(
        Napi::Env env,
        std::string propName, 
        napi_valuetype expected, 
        std::string received) noexcept  {
    return unexpected_type_exception(env, propName, get_napi_type_name(env, expected), received);
}

Napi::Error unexpected_type_exception(
        Napi::Env env,
        std::string propName, 
        std::string expected, 
        std::string received) noexcept  {
    return unexpected_type_exception(env, "Expected type " + expected + " for " + propName
            + " recieved " + received + " instead.");
}

Napi::Error unexpected_type_exception(Napi::Env env, std::string message) noexcept {
    return Napi::Error::New(env, message);
}

missing_callback::missing_callback(std::string message) noexcept : _message(message) { }

const char *missing_callback::what() const noexcept {
    return _message.c_str();
}

void assert_typeof(Napi::Env env, std::string name, Napi::Value val, napi_valuetype type) {
    napi_valuetype actualType;
    if(type != val.Type()) {
        throw unexpected_type_exception(env, name, type, val.Type());
    }
}


template<typename T>
T get_value_from_napi(Napi::Env env, Napi::Value value, std::string name) ;

template<>
bool
get_value_from_napi<bool>(Napi::Env env, Napi::Value value, std::string name) {
    assert_typeof(env, name, value, napi_valuetype::napi_boolean);
    return value.ToBoolean().Value();
}

template<>
std::string 
get_value_from_napi<std::string>(Napi::Env env, Napi::Value value, std::string name) {
    assert_typeof(env, name, value, napi_valuetype::napi_string);
    return value.ToString().Utf8Value();
}

template<>
std::vector<std::string> 
get_value_from_napi<std::vector<std::string>>(
        Napi::Env env, 
        Napi::Value value, 
        std::string name) {
    std::vector<std::string> ret;

    if(!value.IsArray()) {
        throw unexpected_type_exception(env, name, value.Type(), "array");
    }

    Napi::Array arr = value.As<Napi::Array>();

    for(int i = 0; i < arr.Length(); i++) {
        auto val = arr.Get(i);
        ret.push_back(get_value_from_napi<std::string>(env, val, name+"["+std::to_string(i)+"]"));
    }

    return ret;
}

#define ASSIGN_READ(env, dest, val, type, prop)\
    dest.prop = get_value_from_napi<type>(env, val.Get(STR(prop)), STR(prop));


template<>
i18n::addressinput::AddressData 
get_value_from_napi<i18n::addressinput::AddressData>(
        Napi::Env env, 
        Napi::Value val, 
        std::string name) {
    assert_typeof(env, name, val, napi_valuetype::napi_object);

    i18n::addressinput::AddressData data;
    Napi::Object obj = val.ToObject();

    ASSIGN_READ(env, data, obj, std::string, region_code);
    ASSIGN_READ(env, data, obj, std::vector<std::string>, address_line);
    ASSIGN_READ(env, data, obj, std::string, administrative_area);
    ASSIGN_READ(env, data, obj, std::string, locality);
    ASSIGN_READ(env, data, obj, std::string, dependent_locality);
    ASSIGN_READ(env, data, obj, std::string, postal_code);
    ASSIGN_READ(env, data, obj, std::string, sorting_code);
    ASSIGN_READ(env, data, obj, std::string, language_code);
    ASSIGN_READ(env, data, obj, std::string, organization);
    ASSIGN_READ(env, data, obj, std::string, recipient);

    return data;
}

i18n::addressinput::AddressProblem strtoprob(Napi::Env env, const std::string& str) {
    if(str == "UNEXPECTED_FIELD") {
        return i18n::addressinput::AddressProblem::UNEXPECTED_FIELD;
    } else if(str == "USES_P_O_BOX") {
        return i18n::addressinput::AddressProblem::USES_P_O_BOX;
    } else if(str == "UNSUPPORTED_FIELD") {
        return i18n::addressinput::AddressProblem::UNSUPPORTED_FIELD;
    } else if(str == "MISSING_REQUIRED_FIELD") {
        return i18n::addressinput::AddressProblem::MISSING_REQUIRED_FIELD;
    } else if(str == "UNKNOWN_VALUE") {
        return i18n::addressinput::AddressProblem::UNKNOWN_VALUE;
    } else if(str == "MISMATCHING_VALUE") {
        return i18n::addressinput::AddressProblem::MISMATCHING_VALUE;
    } else if(str == "INVALID_FORMAT") {
        return i18n::addressinput::AddressProblem::INVALID_FORMAT;
    } else {
        throw unexpected_type_exception(env, "Expected one of UNEXPECTED_FIELD|USES_P_O_BOX|" 
                "UNSUPPORTED_FIELD|MISSING_REQUIRED_FIELD|UNKNOWN_VALUE|MISMATCHING_VALUE|"
                "INVALID_FORMAT, recieved " + str);
    }
}

i18n::addressinput::AddressField strtofield(Napi::Env env, const std::string& str) {
    if(str == "STREET_ADDRESS") {
        return i18n::addressinput::AddressField::STREET_ADDRESS;
    } else if(str == "ADMIN_AREA") {
        return i18n::addressinput::AddressField::ADMIN_AREA;
    } else if(str == "COUNTRY") {
        return i18n::addressinput::AddressField::COUNTRY;
    } else if(str == "DEPENDENT_LOCALITY") {
        return i18n::addressinput::AddressField::DEPENDENT_LOCALITY;
    } else if(str == "LOCALITY") {
        return i18n::addressinput::AddressField::LOCALITY;
    } else if(str == "ORGANIZATION") {
        return i18n::addressinput::AddressField::ORGANIZATION;
    } else if(str == "POSTAL_CODE") {
        return i18n::addressinput::AddressField::POSTAL_CODE;
    } else if(str == "SORTING_CODE") {
        return i18n::addressinput::AddressField::SORTING_CODE;
    } else if(str == "RECIPIENT") {
        return i18n::addressinput::AddressField::RECIPIENT;
    } else {
        throw unexpected_type_exception(env, "Expected one of UNEXPECTED_FIELD|USES_P_O_BOX|" 
                "UNSUPPORTED_FIELD|MISSING_REQUIRED_FIELD|UNKNOWN_VALUE|MISMATCHING_VALUE|"
                "INVALID_FORMAT, recieved " + str);
    }
}

template<>
i18n::addressinput::FieldProblemMap
get_value_from_napi<i18n::addressinput::FieldProblemMap>(
        Napi::Env env, 
        Napi::Value val, 
        std::string name) {
    assert_typeof(env, name, val, napi_valuetype::napi_object);

    i18n::addressinput::FieldProblemMap ret;

    for(auto item : val.As<Napi::Object>()) {
        auto key = get_value_from_napi<std::string>(env, item.first, name+".<key>");

        auto items = get_value_from_napi<std::vector<std::string>>(
                env, 
                item.second, 
                name+"["+key+"]");

        for(auto str : items) {
            ret.insert({strtofield(env, key), strtoprob(env, str)});
        }
    }

    return ret;
}

Napi::Value to_napi_value(Napi::Env env, const std::string& str) {
    return Napi::String::New(env, str);
}

Napi::Value to_napi_value(Napi::Env env, const i18n::addressinput::AddressProblem& problem) {
    std::string str;
    switch(problem) {
        case i18n::addressinput::AddressProblem::INVALID_FORMAT:
            str = "INVALID_FORMAT";
            break;

        case i18n::addressinput::AddressProblem::MISMATCHING_VALUE:
            str = "MISMATCHING_VALUE";
            break;

        case i18n::addressinput::AddressProblem::UNKNOWN_VALUE:
            str = "UNKNOWN_VALUE";
            break;

        case i18n::addressinput::AddressProblem::MISSING_REQUIRED_FIELD:
            str = "MISSING_REQUIRED_FIELD";
            break;

        case i18n::addressinput::AddressProblem::UNEXPECTED_FIELD:
            str = "UNEXPECTED_FIELD";
            break;

        case i18n::addressinput::AddressProblem::UNSUPPORTED_FIELD:
            str = "UNSUPPORTED_FIELD";
            break;

        case i18n::addressinput::AddressProblem::USES_P_O_BOX:
            str = "USES_P_O_BOX";
            break;
    }
    return to_napi_value(env, str);
}

Napi::Value to_napi_value(Napi::Env env, const i18n::addressinput::FieldProblemMap& problems) {
    Napi::Object retval = Napi::Object::New(env);

    for(auto set : problems) {
        if(!retval.Has(set.first)) {
            retval.Set(set.first, Napi::Array::New(env));
        }
        std::stringstream ss;
        ss << set.first; //Serialize AddressField
        Napi::Array arr = retval.Get(ss.str()).As<Napi::Array>();
        Napi::Value func = arr.Get("push");
        assert_typeof(env, "push", func, napi_valuetype::napi_function);
        func.As<Napi::Function>().Call(arr, {to_napi_value(env, set.second)});
    }

    return retval;
}

Napi::Value to_napi_value(Napi::Env env, const std::vector<std::string>& strs) {
    Napi::Array arr = Napi::Array::New(env);
    Napi::Value func = arr.Get("push");
    assert_typeof(env, "push", func, napi_valuetype::napi_function);
    auto push = func.As<Napi::Function>();

    for(auto str : strs) {
        push.Call(arr, {to_napi_value(env, str)});
    }

    return arr;
}

Napi::Value to_napi_value(Napi::Env env, const i18n::addressinput::AddressData& problems) {
    Napi::Object ret = Napi::Object::New(env);

    ret.Set("region_code", to_napi_value(env, problems.region_code));
    ret.Set("language_code", to_napi_value(env, problems.language_code));
    ret.Set("postal_code", to_napi_value(env, problems.postal_code));
    ret.Set("sorting_code", to_napi_value(env, problems.sorting_code));
    ret.Set("administrative_area", to_napi_value(env, problems.administrative_area));
    ret.Set("dependent_locality", to_napi_value(env, problems.dependent_locality));
    ret.Set("locality", to_napi_value(env, problems.locality));
    ret.Set("organization", to_napi_value(env, problems.organization));
    ret.Set("recipient", to_napi_value(env, problems.recipient));
    ret.Set("address_line", to_napi_value(env, problems.address_line));

    return ret;
}

Napi::Value to_napi_value(
        Napi::Env env, 
        const std::pair<
            const i18n::addressinput::AddressData&, 
            const i18n::addressinput::FieldProblemMap&>& pair) {
    Napi::Array arr;
    Napi::Value func = arr.Get("push");
    assert_typeof(env, "push", func, napi_valuetype::napi_function);
    auto push = func.As<Napi::Function>();

    push.Call(arr, { to_napi_value(env, pair.first) });
    push.Call(arr, { to_napi_value(env, pair.second) });

    return arr;
}


void then(Napi::Promise promise, std::function<void (const Napi::CallbackInfo&)> callback) {
    auto then = promise.Get("then");
    if(!then.IsFunction()) {
        throw unexpected_type_exception(promise.Env(), "promise is not thenable");
    }

    then.As<Napi::Function>().Call(promise, {Napi::Function::New(promise.Env(), callback)});
}

void handle_get_result(Napi::Env env, const std::string& key, Napi::Value result, const i18n::addressinput::Source::Callback& data_ready) {
    auto cb = [&data_ready, key](Napi::Value v) {
        try {
            if(v.IsString()) {
                std::string *data = new std::string(v.ToString().Utf8Value());
                data_ready(true, key, data);
            } else if(v.IsNull()) {
                data_ready(false, key, nullptr);
            } else {
                throw unexpected_type_exception(
                        v.Env(), "storage_get_result", napi_valuetype::napi_string, v.Type());
            }
        } catch(Napi::Error& err) {
            data_ready(false, key, nullptr);
            throw;
        }
    };

    if(result.IsPromise()) {
        auto promise = result.As<Napi::Promise>();
        then(promise, [&cb](const Napi::CallbackInfo& info) {
            if(info.Length() <= 0 || info[0].IsNull() || info[0].IsUndefined()) {
                throw unexpected_type_exception(info.Env(), "Expected promise to return a value");
            }

            cb(info[0]);
        });
    }

    cb(result);
}

void i18n::addressinput::JsDelegatedSource::Get(const std::string& key, const Callback& data_ready) const {
    if(!_get) throw missing_callback("No source callback registered");

    try {
        auto result = _get->Call({Napi::String::New(_get->Env(), key)});
        handle_get_result(_get->Env(), key, result, data_ready);
    } catch(Napi::Error& err) {
        data_ready(false, key, nullptr);
        throw;
    }
}

void i18n::addressinput::JsDelegatedSource::SetAcquisition(Napi::Function func) {
    _get = Napi::Persistent(func);
}

void i18n::addressinput::JsDelegatedStorage::Get(const std::string& key, const Callback& data_ready) const {
    if(!_get) throw missing_callback("No storage retrieve callback registered");

    try {
        auto result = _get->Call({Napi::String::New(_get->Env(), key)});
        handle_get_result(_get->Env(), key, result, data_ready);
    } catch(Napi::Error& err) {
        data_ready(false, key, nullptr);
        throw;
    }
}

void i18n::addressinput::JsDelegatedStorage::SetAcquisition(Napi::Function func) {
    _get = Napi::Persistent(func);
}

void i18n::addressinput::JsDelegatedStorage::Put(const std::string &key, std::string *data) {
    if(!_put) throw missing_callback("No storage save callback registered");

    auto result = _put->Call({
            Napi::String::New(_put->Env(), key), 
            Napi::String::New(_put->Env(), *data)});
}

void i18n::addressinput::JsDelegatedStorage::SetStore(Napi::Function func) {
    _put = Napi::Persistent(func);
}

template<typename Key, typename Data>
class FunctionCallbackWrapper : public i18n::addressinput::Callback<Key, Data> {
public:
    FunctionCallbackWrapper(std::function<void (bool success, Key a1, Data a2)> func) : _func(func) { }

    virtual void operator()(bool success, Key a1, Data a2) const override {
        _func(success, a1, a2);
    }

private:
    std::function<void (bool success, Key a1, Data a2)> _func;
};

JsAddressValidator::JsAddressValidator(const Napi::CallbackInfo& info) 
        : Napi::ObjectWrap<JsAddressValidator>(info)
        , _source(new i18n::addressinput::JsDelegatedSource())
        , _storage(new i18n::addressinput::JsDelegatedStorage())
        , _supplier(_source, _storage)
        , _validator(&_supplier) {
    if(info.Length() <= 0) {
        throw unexpected_type_exception(info.Env(), "Expected an object in arguments");
    }

    assert_typeof(info.Env(), "config", info[0], napi_valuetype::napi_object);
    auto config = info[0].ToObject();

    auto source = config.Get("request");
    auto cache = config.Get("put");
    auto retrieve = config.Get("get");

    if(!source.IsUndefined()) {
        assert_typeof(info.Env(), "request", source, napi_valuetype::napi_function);
        _source->SetAcquisition(source.As<Napi::Function>());
    }
    if(!cache.IsUndefined()) {
        assert_typeof(info.Env(), "put", cache, napi_valuetype::napi_function);
        _storage->SetStore(cache.As<Napi::Function>());
    }
    if(!retrieve.IsUndefined()) {
        assert_typeof(info.Env(), "get", retrieve, napi_valuetype::napi_function);
        _storage->SetAcquisition(retrieve.As<Napi::Function>());
    }
}

Napi::FunctionReference JsAddressValidator::constructor;

Napi::Object JsAddressValidator::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    auto func = DefineClass(env, "AddressValidator", {
        InstanceMethod("validate", &JsAddressValidator::validate_address)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("AddressValidator", func);
    return exports;
}

using ValidateCallback = FunctionCallbackWrapper<
    const i18n::addressinput::AddressData&, const i18n::addressinput::FieldProblemMap&>;

Napi::Value JsAddressValidator::validate_address(const Napi::CallbackInfo& info) {
    size_t argc = info.Length();
    Napi::Value result;

    if(info.Length() <= 1) {
        throw unexpected_type_exception(info.Env(), "Expected an object in arguments");
    }

    auto address = get_value_from_napi<i18n::addressinput::AddressData>(info.Env(), info[0], "address");
    auto conf = info[1].ToObject();

    auto allow_postal = get_value_from_napi<bool>(info.Env(), conf.Get("allow_postal"), "allow_postal");
    auto require_name = get_value_from_napi<bool>(info.Env(), conf.Get("require_name"), "require_name");
    auto filter = get_value_from_napi<i18n::addressinput::FieldProblemMap>(info.Env(), conf.Get("filter"), "filter");

    //Heap allocate filter due to pointer requirement
    auto filterPtr = std::make_shared<i18n::addressinput::FieldProblemMap>(filter);
    auto problems = std::make_shared<i18n::addressinput::FieldProblemMap>();
    auto defer = Napi::Promise::Deferred::New(info.Env());

    ValidateCallback cb{[defer, problems, filterPtr](
        bool success, 
        const i18n::addressinput::AddressData& data,
        const i18n::addressinput::FieldProblemMap& resultProblems
    ) {
        if(success) {
            defer.Resolve(to_napi_value(defer.Env(), std::make_pair(data, resultProblems)));
        } else {
            defer.Reject(Napi::Error::New(defer.Env(), "Validator call failed").Value());
        }
    }};

    _validator.Validate(address, allow_postal, require_name, filterPtr.get(), problems.get(), cb);

    return defer.Promise();
}
