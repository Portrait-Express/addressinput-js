#include "address_validator.h"
#include <csignal>
#include <exception>
#include <napi.h>

#include <iostream>

#define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>

#ifdef __linux__

#include <link.h>

int callback(struct dl_phdr_info *info, size_t size, void *data) {
    printf("%s @ %#lx\n", info->dlpi_name, (unsigned long)info->dlpi_addr);
    return 0;
}

void print_program_bases() {
    std::cout << "PROGRAM BASES: " << std::endl;
    dl_iterate_phdr(&callback, NULL);
}

#endif

void handler() {
#ifdef __linux__
    print_program_bases();
#endif

    std::cout << "STACK:\n" << boost::stacktrace::stacktrace();

    exit(1);
}

void segvhandler(int code) {
    handler();
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    std::set_terminate(handler);
    std::signal(SIGSEGV, segvhandler);
    std::signal(SIGBUS, segvhandler);
    std::signal(SIGABRT, segvhandler);
    return JsAddressValidator::Init(env, exports);
}

NODE_API_MODULE(addressinput, InitAll);
