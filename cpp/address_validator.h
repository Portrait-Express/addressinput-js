#ifndef INCLUDE_CPP_LIBADDRESSINPUT_TS_H_
#define INCLUDE_CPP_LIBADDRESSINPUT_TS_H_


#include <optional>
#include <string>

#include <napi.h>

#include <libaddressinput/source.h>
#include <libaddressinput/address_validator.h>
#include <libaddressinput/ondemand_supplier.h>
#include <libaddressinput/storage.h>

#define STR(v) _STR(v)
#define _STR(v) #v

Napi::Error unexpected_type_exception(Napi::Env env, std::string propName, napi_valuetype expected, napi_valuetype recieved) noexcept;
Napi::Error unexpected_type_exception(Napi::Env env, std::string propName, napi_valuetype expected, std::string recieved) noexcept;
Napi::Error unexpected_type_exception(Napi::Env env, std::string propName, std::string expected, napi_valuetype recieved) noexcept;
Napi::Error unexpected_type_exception(Napi::Env env, std::string propName, std::string expected, std::string recieved) noexcept;
Napi::Error unexpected_type_exception(Napi::Env env, std::string message) noexcept;


class missing_callback : public std::exception {
public:
    missing_callback(std::string message) noexcept;

    const char * what() const noexcept override;

private:
    std::string _message;
};

namespace i18n {
namespace addressinput {

class JsDelegatedSource : public Source {
public:
    void Get(const std::string& key, const Callback& data_ready) const override;

    void SetAcquisition(Napi::Function cb);

private:
    std::optional<Napi::FunctionReference> _get;
};

class JsDelegatedStorage : public Storage {
public:
    void Get(const std::string& key, const Callback& data_ready) const override;
    void Put(const std::string &key, std::string *data) override;

    void SetAcquisition(Napi::Function cb);
    void SetStore(Napi::Function cb);

private:
    std::optional<Napi::FunctionReference> _put;
    std::optional<Napi::FunctionReference> _get;
};

}
}

class JsAddressValidator : public Napi::ObjectWrap<JsAddressValidator> {
public:
    JsAddressValidator(const Napi::CallbackInfo& info);
    static Napi::Object Init(Napi::Env, Napi::Object exports);

    Napi::Value validate_address(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;
    
    i18n::addressinput::JsDelegatedSource *_source;
    i18n::addressinput::JsDelegatedStorage *_storage;
    i18n::addressinput::OndemandSupplier _supplier;
    i18n::addressinput::AddressValidator _validator;
};



#endif  // INCLUDE_CPP_LIBADDRESSINPUT-TS_H_
