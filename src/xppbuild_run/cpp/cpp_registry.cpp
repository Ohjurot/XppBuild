#include "cpp_registry.h"

xpp::run::cpp::CppProviderRegistry xpp::run::cpp::CppProviderRegistry::singleton_instance;

void xpp::run::cpp::CppProviderRegistry::init()
{
    // Register providers
    register_provider<MsvcProvider>("msvc");
    register_provider<ClangProvider>("clang");
}

xpp::run::cpp::ICppProvider* xpp::run::cpp::CppProviderRegistry::get_provider(const std::string& name)
{
    ICppProvider* provider = nullptr;
    auto it_find = singleton_instance.cpp_registry.find(name);
    if (it_find != singleton_instance.cpp_registry.end())
    {
        provider = it_find->second;
    }

    return provider;
}
