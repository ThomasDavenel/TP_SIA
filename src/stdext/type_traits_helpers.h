#pragma once

/// <summary> 
/// This macro declares a method named static constexpr bool has_##methodName&lt;Class&gt;() that tests if a class Class has a method returnType methodName(...)
/// </summary> 
#define macro_has_method(methodName, returnType, ...) \
template <typename Class> \
static constexpr bool has_##methodName (decltype(Class::methodName) *) \
{ return std::is_assignable<std::function<returnType (__VA_ARGS__)>, decltype(Class::methodName)>::value; } \
template <typename Class> \
static constexpr bool has_##methodName (...) { return false;} \
template <typename Class> \
static constexpr bool has_##methodName() { return has_##methodName<Class>(nullptr);}

/// <summary> 
/// This macro declares a method named static constexpr bool has_##type&lt;Class&gt;() that tests if a class Class has an internal type "type" 
/// </summary> 
#define macro_has_type(type) \
template <typename Class> \
static constexpr bool has_##type(typename Class::type *) { return true; } \
template <typename Class> \
static constexpr bool has_##type(...) { return false; } \
template <typename Class> \
static constexpr bool has_##type() { return has_##type<Class>(nullptr); }

