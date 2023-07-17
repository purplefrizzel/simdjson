#ifndef SIMDJSON_GENERIC_SINGLESTAGE_VALUE_INL_H

#ifndef SIMDJSON_AMALGAMATED
#define SIMDJSON_GENERIC_SINGLESTAGE_VALUE_INL_H
#include "simdjson/generic/singlestage/base.h"
#include "simdjson/generic/singlestage/array.h"
#include "simdjson/generic/singlestage/array_iterator.h"
#include "simdjson/generic/singlestage/json_iterator.h"
#include "simdjson/generic/singlestage/json_type.h"
#include "simdjson/generic/singlestage/object.h"
#include "simdjson/generic/singlestage/raw_json_string.h"
#include "simdjson/generic/singlestage/value.h"
#endif // SIMDJSON_AMALGAMATED

namespace simdjson {
namespace SIMDJSON_IMPLEMENTATION {
namespace singlestage {

simdjson_inline value::value(const value_iterator &_iter) noexcept
  : iter{_iter}
{
}
simdjson_inline value value::start(const value_iterator &iter) noexcept {
  return iter;
}
simdjson_inline value value::resume(const value_iterator &iter) noexcept {
  return iter;
}

simdjson_inline simdjson_result<array> value::get_array() noexcept {
  return array::start(iter);
}
simdjson_inline simdjson_result<object> value::get_object() noexcept {
  return object::start(iter);
}
simdjson_inline simdjson_result<object> value::start_or_resume_object() noexcept {
  if (iter.at_start()) {
    return get_object();
  } else {
    return object::resume(iter);
  }
}

simdjson_inline simdjson_result<raw_json_string> value::get_raw_json_string() noexcept {
  return iter.get_raw_json_string();
}
simdjson_inline simdjson_result<std::string_view> value::get_string(bool allow_replacement) noexcept {
  return iter.get_string(allow_replacement);
}
simdjson_inline simdjson_result<std::string_view> value::get_wobbly_string() noexcept {
  return iter.get_wobbly_string();
}
simdjson_inline simdjson_result<double> value::get_double() noexcept {
  return iter.get_double();
}
simdjson_inline simdjson_result<double> value::get_double_in_string() noexcept {
  return iter.get_double_in_string();
}
simdjson_inline simdjson_result<uint64_t> value::get_uint64() noexcept {
  return iter.get_uint64();
}
simdjson_inline simdjson_result<uint64_t> value::get_uint64_in_string() noexcept {
  return iter.get_uint64_in_string();
}
simdjson_inline simdjson_result<int64_t> value::get_int64() noexcept {
  return iter.get_int64();
}
simdjson_inline simdjson_result<int64_t> value::get_int64_in_string() noexcept {
  return iter.get_int64_in_string();
}
simdjson_inline simdjson_result<bool> value::get_bool() noexcept {
  return iter.get_bool();
}
simdjson_inline simdjson_result<bool> value::is_null() noexcept {
  return iter.is_null();
}
template<> simdjson_inline simdjson_result<array> value::get() noexcept { return get_array(); }
template<> simdjson_inline simdjson_result<object> value::get() noexcept { return get_object(); }
template<> simdjson_inline simdjson_result<raw_json_string> value::get() noexcept { return get_raw_json_string(); }
template<> simdjson_inline simdjson_result<std::string_view> value::get() noexcept { return get_string(false); }
template<> simdjson_inline simdjson_result<number> value::get() noexcept { return get_number(); }
template<> simdjson_inline simdjson_result<double> value::get() noexcept { return get_double(); }
template<> simdjson_inline simdjson_result<uint64_t> value::get() noexcept { return get_uint64(); }
template<> simdjson_inline simdjson_result<int64_t> value::get() noexcept { return get_int64(); }
template<> simdjson_inline simdjson_result<bool> value::get() noexcept { return get_bool(); }

template<typename T> simdjson_inline error_code value::get(T &out) noexcept {
  return get<T>().get(out);
}

#if SIMDJSON_EXCEPTIONS
simdjson_inline value::operator array() noexcept(false) {
  return get_array();
}
simdjson_inline value::operator object() noexcept(false) {
  return get_object();
}
simdjson_inline value::operator uint64_t() noexcept(false) {
  return get_uint64();
}
simdjson_inline value::operator int64_t() noexcept(false) {
  return get_int64();
}
simdjson_inline value::operator double() noexcept(false) {
  return get_double();
}
simdjson_inline value::operator std::string_view() noexcept(false) {
  return get_string(false);
}
simdjson_inline value::operator raw_json_string() noexcept(false) {
  return get_raw_json_string();
}
simdjson_inline value::operator bool() noexcept(false) {
  return get_bool();
}
#endif

simdjson_inline simdjson_result<array_iterator> value::begin() & noexcept {
  return get_array().begin();
}
simdjson_inline simdjson_result<array_iterator> value::end() & noexcept {
  return {};
}
simdjson_inline simdjson_result<size_t> value::count_elements() & noexcept {
  simdjson_result<size_t> answer;
  auto a = get_array();
  answer = a.count_elements();
  // count_elements leaves you pointing inside the array, at the first element.
  // We need to move back so that the user can create a new array (which requires that
  // we point at '[').
  iter.move_at_start();
  return answer;
}
simdjson_inline simdjson_result<size_t> value::count_fields() & noexcept {
  simdjson_result<size_t> answer;
  auto a = get_object();
  answer = a.count_fields();
  iter.move_at_start();
  return answer;
}
simdjson_inline simdjson_result<value> value::at(size_t index) noexcept {
  auto a = get_array();
  return a.at(index);
}

simdjson_inline simdjson_result<value> value::find_field(std::string_view key) noexcept {
  return start_or_resume_object().find_field(key);
}
simdjson_inline simdjson_result<value> value::find_field(const char *key) noexcept {
  return start_or_resume_object().find_field(key);
}

simdjson_inline simdjson_result<value> value::find_field_unordered(std::string_view key) noexcept {
  return start_or_resume_object().find_field_unordered(key);
}
simdjson_inline simdjson_result<value> value::find_field_unordered(const char *key) noexcept {
  return start_or_resume_object().find_field_unordered(key);
}

simdjson_inline simdjson_result<value> value::operator[](std::string_view key) noexcept {
  return start_or_resume_object()[key];
}
simdjson_inline simdjson_result<value> value::operator[](const char *key) noexcept {
  return start_or_resume_object()[key];
}

simdjson_inline simdjson_result<json_type> value::type() noexcept {
  return iter.type();
}

simdjson_inline simdjson_result<bool> value::is_scalar() noexcept {
  json_type this_type;
  auto error = type().get(this_type);
  if(error) { return error; }
  return ! ((this_type == json_type::array) || (this_type == json_type::object));
}

simdjson_inline bool value::is_negative() noexcept {
  return iter.is_negative();
}

simdjson_inline simdjson_result<bool> value::is_integer() noexcept {
  return iter.is_integer();
}
simdjson_warn_unused simdjson_inline simdjson_result<number_type> value::get_number_type() noexcept {
  return iter.get_number_type();
}
simdjson_warn_unused simdjson_inline simdjson_result<number> value::get_number() noexcept {
  return iter.get_number();
}

simdjson_inline std::string_view value::raw_json_token() noexcept {
  return std::string_view(reinterpret_cast<const char*>(iter.peek_start()), iter.peek_start_length());
}

simdjson_inline simdjson_result<const char *> value::current_location() noexcept {
  return iter.json_iter().current_location();
}

simdjson_inline int32_t value::current_depth() const noexcept{
  return iter.json_iter().depth();
}

simdjson_inline simdjson_result<value> value::at_pointer(std::string_view json_pointer) noexcept {
  json_type t;
  SIMDJSON_TRY(type().get(t));
  switch (t)
  {
    case json_type::array:
      return (*this).get_array().at_pointer(json_pointer);
    case json_type::object:
      return (*this).get_object().at_pointer(json_pointer);
    default:
      return INVALID_JSON_POINTER;
  }
}

} // namespace singlestage
} // namespace SIMDJSON_IMPLEMENTATION
} // namespace simdjson

namespace simdjson {

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::simdjson_result(
  SIMDJSON_IMPLEMENTATION::singlestage::value &&value
) noexcept :
    implementation_simdjson_result_base<SIMDJSON_IMPLEMENTATION::singlestage::value>(
      std::forward<SIMDJSON_IMPLEMENTATION::singlestage::value>(value)
    )
{
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::simdjson_result(
  error_code error
) noexcept :
    implementation_simdjson_result_base<SIMDJSON_IMPLEMENTATION::singlestage::value>(error)
{
}
simdjson_inline simdjson_result<size_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::count_elements() & noexcept {
  if (error()) { return error(); }
  return first.count_elements();
}
simdjson_inline simdjson_result<size_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::count_fields() & noexcept {
  if (error()) { return error(); }
  return first.count_fields();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::at(size_t index) noexcept {
  if (error()) { return error(); }
  return first.at(index);
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::array_iterator> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::begin() & noexcept {
  if (error()) { return error(); }
  return first.begin();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::array_iterator> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::end() & noexcept {
  if (error()) { return error(); }
  return {};
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::find_field(std::string_view key) noexcept {
  if (error()) { return error(); }
  return first.find_field(key);
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::find_field(const char *key) noexcept {
  if (error()) { return error(); }
  return first.find_field(key);
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::find_field_unordered(std::string_view key) noexcept {
  if (error()) { return error(); }
  return first.find_field_unordered(key);
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::find_field_unordered(const char *key) noexcept {
  if (error()) { return error(); }
  return first.find_field_unordered(key);
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator[](std::string_view key) noexcept {
  if (error()) { return error(); }
  return first[key];
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator[](const char *key) noexcept {
  if (error()) { return error(); }
  return first[key];
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::array> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_array() noexcept {
  if (error()) { return error(); }
  return first.get_array();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::object> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_object() noexcept {
  if (error()) { return error(); }
  return first.get_object();
}
simdjson_inline simdjson_result<uint64_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_uint64() noexcept {
  if (error()) { return error(); }
  return first.get_uint64();
}
simdjson_inline simdjson_result<uint64_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_uint64_in_string() noexcept {
  if (error()) { return error(); }
  return first.get_uint64_in_string();
}
simdjson_inline simdjson_result<int64_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_int64() noexcept {
  if (error()) { return error(); }
  return first.get_int64();
}
simdjson_inline simdjson_result<int64_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_int64_in_string() noexcept {
  if (error()) { return error(); }
  return first.get_int64_in_string();
}
simdjson_inline simdjson_result<double> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_double() noexcept {
  if (error()) { return error(); }
  return first.get_double();
}
simdjson_inline simdjson_result<double> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_double_in_string() noexcept {
  if (error()) { return error(); }
  return first.get_double_in_string();
}
simdjson_inline simdjson_result<std::string_view> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_string(bool allow_replacement) noexcept {
  if (error()) { return error(); }
  return first.get_string(allow_replacement);
}
simdjson_inline simdjson_result<std::string_view> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_wobbly_string() noexcept {
  if (error()) { return error(); }
  return first.get_wobbly_string();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::raw_json_string> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_raw_json_string() noexcept {
  if (error()) { return error(); }
  return first.get_raw_json_string();
}
simdjson_inline simdjson_result<bool> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_bool() noexcept {
  if (error()) { return error(); }
  return first.get_bool();
}
simdjson_inline simdjson_result<bool> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::is_null() noexcept {
  if (error()) { return error(); }
  return first.is_null();
}

template<typename T> simdjson_inline simdjson_result<T> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get() noexcept {
  if (error()) { return error(); }
  return first.get<T>();
}
template<typename T> simdjson_inline error_code simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get(T &out) noexcept {
  if (error()) { return error(); }
  return first.get<T>(out);
}

template<> simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get<SIMDJSON_IMPLEMENTATION::singlestage::value>() noexcept  {
  if (error()) { return error(); }
  return std::move(first);
}
template<> simdjson_inline error_code simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get<SIMDJSON_IMPLEMENTATION::singlestage::value>(SIMDJSON_IMPLEMENTATION::singlestage::value &out) noexcept {
  if (error()) { return error(); }
  out = first;
  return SUCCESS;
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::json_type> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::type() noexcept {
  if (error()) { return error(); }
  return first.type();
}
simdjson_inline simdjson_result<bool> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::is_scalar() noexcept {
  if (error()) { return error(); }
  return first.is_scalar();
}
simdjson_inline simdjson_result<bool> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::is_negative() noexcept {
  if (error()) { return error(); }
  return first.is_negative();
}
simdjson_inline simdjson_result<bool> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::is_integer() noexcept {
  if (error()) { return error(); }
  return first.is_integer();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::number_type> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_number_type() noexcept {
  if (error()) { return error(); }
  return first.get_number_type();
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::number> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::get_number() noexcept {
  if (error()) { return error(); }
  return first.get_number();
}
#if SIMDJSON_EXCEPTIONS
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator SIMDJSON_IMPLEMENTATION::singlestage::array() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator SIMDJSON_IMPLEMENTATION::singlestage::object() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator uint64_t() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator int64_t() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator double() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator std::string_view() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator SIMDJSON_IMPLEMENTATION::singlestage::raw_json_string() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::operator bool() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first;
}
#endif

simdjson_inline simdjson_result<std::string_view> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::raw_json_token() noexcept {
  if (error()) { return error(); }
  return first.raw_json_token();
}

simdjson_inline simdjson_result<const char *> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::current_location() noexcept {
  if (error()) { return error(); }
  return first.current_location();
}

simdjson_inline simdjson_result<int32_t> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::current_depth() const noexcept {
  if (error()) { return error(); }
  return first.current_depth();
}

simdjson_inline simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value> simdjson_result<SIMDJSON_IMPLEMENTATION::singlestage::value>::at_pointer(std::string_view json_pointer) noexcept {
  if (error()) { return error(); }
  return first.at_pointer(json_pointer);
}

} // namespace simdjson

#endif // SIMDJSON_GENERIC_SINGLESTAGE_VALUE_INL_H