#pragma once

#include "cpp_provider.h"
#include "msvc/cpp_msvc_provider.h"
#include "clang/cpp_clang_provider.h"

#include <string>
#include <unordered_map>
#include <type_traits>

namespace xpp::run::cpp
{
    // All provider will be registered in here
    class CppProviderRegistry
    {
        public:
            // Delete unsupported
            CppProviderRegistry(const CppProviderRegistry&) noexcept = delete;
            CppProviderRegistry& operator=(const CppProviderRegistry&) noexcept = delete;

            // Will init the class
            static void init();

            // static
            static ICppProvider* get_provider(const std::string& name);

        private:
            // Helper
            template<typename T, typename = std::enable_if_t<std::is_base_of_v<ICppProvider, T> && std::is_default_constructible_v<T>>>
            static void register_provider(const std::string& name)
            {
                // Cleanup
                auto find_it = singleton_instance.cpp_registry.find(name);
                if (find_it != singleton_instance.cpp_registry.end())
                {
                    delete find_it->second;
                    singleton_instance.cpp_registry.erase(find_it);
                }

                // Register
                singleton_instance.cpp_registry[name] = new T;
            }

        private:
            // Singleton
            CppProviderRegistry() noexcept {};
            static CppProviderRegistry singleton_instance;

            // Assign map
            std::map<std::string, ICppProvider*> cpp_registry;
    };
}
