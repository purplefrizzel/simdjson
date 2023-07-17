#include "simdjson.h"
#include "test_singlestage.h"

using namespace simdjson;

namespace object_error_tests {
  using namespace std;

  template<typename V, typename T>
  bool assert_iterate_object(T &&object, const char **expected_key, V *expected, size_t N, simdjson::error_code *expected_error, size_t N2) {
    size_t count = 0;
    for (auto field : object) {
      V actual{};
      auto actual_error = field.value().get(actual);
      if (count >= N) {
        ASSERT((count - N) < N2, "Extra error reported");
        ASSERT_ERROR(actual_error, expected_error[count - N]);
      } else {
        ASSERT_SUCCESS(actual_error);
        ASSERT_EQUAL(field.key().value_unsafe(), expected_key[count]);
        ASSERT_EQUAL(actual, expected[count]);
      }
      count++;
    }
    ASSERT_EQUAL(count, N+N2);
    return true;
  }

  template<typename V, size_t N, size_t N2, typename T>
  bool assert_iterate_object(T &&object, const char *(&&expected_key)[N], V (&&expected)[N], simdjson::error_code (&&expected_error)[N2]) {
    return assert_iterate_object<V, T>(std::forward<T>(object), expected_key, expected, N, expected_error, N2);
  }

  template<size_t N2, typename T>
  bool assert_iterate_object(T &&object, simdjson::error_code (&&expected_error)[N2]) {
    return assert_iterate_object<int64_t, T>(std::forward<T>(object), nullptr, nullptr, 0, expected_error, N2);
  }

  template<typename V, size_t N, typename T>
  bool assert_iterate_object(T &&object, const char *(&&expected_key)[N], V (&&expected)[N]) {
    return assert_iterate_object<V, T>(std::forward<T>(object), expected_key, expected, N, nullptr, 0);
  }

  bool object_iterate_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("missing colon", R"({ "a"  1, "b": 2 })",    assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("missing key  ", R"({    : 1, "b": 2 })",    assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("missing value", R"({ "a":  , "b": 2 })",    assert_iterate_object(doc.get_object(),                          { INCORRECT_TYPE, TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("missing comma", R"({ "a": 1  "b": 2 })",    assert_iterate_object(doc.get_object(), { "a" }, { int64_t(1) }, { TAPE_ERROR }));
    TEST_SUCCEED();
  }
  bool object_iterate_wrong_key_type_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("wrong key type", R"({ 1:   1, "b": 2 })",    assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ true: 1, "b": 2 })",   assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ false: 1, "b": 2 })",  assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ null: 1, "b": 2 })",   assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ []:  1, "b": 2 })",    assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ {}:  1, "b": 2 })",    assert_iterate_object(doc.get_object(),                          { TAPE_ERROR }));
    TEST_SUCCEED();
  }
  bool object_iterate_unclosed_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a": 1,         )",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a": 1          )",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a":            )",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a"             )",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({                 )",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    TEST_SUCCEED();
  }
  bool object_iterate_incomplete_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("unclosed", R"({ "x": { "a": 1, })",    assert_iterate_object(doc.get_object(), { "a" }, { int64_t(1) }, { TAPE_ERROR }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "x": { "a": 1  })",    assert_iterate_object(doc.get_object(), { "a" }, { int64_t(1) }, { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "x": { "a":    })",    assert_iterate_object(doc.get_object(), { INCORRECT_TYPE, INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "x": { "a"     })",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "x": {         })",    assert_iterate_object(doc.get_object(), { INCOMPLETE_ARRAY_OR_OBJECT }));
    TEST_SUCCEED();
  }

  bool object_lookup_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("missing colon", R"({ "a"  1, "b": 2 })",    assert_error(doc["a"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("missing key  ", R"({    : 1, "b": 2 })",    assert_error(doc["a"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("missing value", R"({ "a":  , "b": 2 })",    assert_success(doc["a"]));
    SINGLESTAGE_SUBTEST("missing comma", R"({ "a": 1  "b": 2 })",    assert_success(doc["a"]));
    TEST_SUCCEED();
  }
  bool object_lookup_unclosed_error() {
    TEST_START();
  #if SIMDJSON_CHECK_EOF
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a":            )",    assert_error(doc["a"], INCOMPLETE_ARRAY_OR_OBJECT));
  #else
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a":            )",    assert_error(doc["a"], INCOMPLETE_ARRAY_OR_OBJECT));
  #endif
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a"             )",    assert_error(doc["a"], INCOMPLETE_ARRAY_OR_OBJECT));
    SINGLESTAGE_SUBTEST("unclosed", R"({                 )",    assert_error(doc["a"], INCOMPLETE_ARRAY_OR_OBJECT));
    TEST_SUCCEED();
  }

  bool object_lookup_miss_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("missing colon", R"({ "a"  1, "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("missing key  ", R"({    : 1, "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("missing value", R"({ "a":  , "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("missing comma", R"({ "a": 1  "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    TEST_SUCCEED();
  }
  bool object_lookup_miss_wrong_key_type_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("wrong key type", R"({ 1:   1, "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ true: 1, "b": 2 })",   assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ false: 1, "b": 2 })",  assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ null: 1, "b": 2 })",   assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ []:  1, "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    SINGLESTAGE_SUBTEST("wrong key type", R"({ {}:  1, "b": 2 })",    assert_error(doc["b"], TAPE_ERROR));
    TEST_SUCCEED();
  }
  bool object_lookup_miss_unclosed_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a": 1,         )",    assert_error(doc["b"], INCOMPLETE_ARRAY_OR_OBJECT));
    // TODO These next two pass the user a value that may run past the end of the buffer if they aren't careful.
    // In particular, if the padding is decorated with the wrong values, we could cause overrun!
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a": 1          )",    assert_error(doc["b"], INCOMPLETE_ARRAY_OR_OBJECT));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a":            )",    assert_error(doc["b"], INCOMPLETE_ARRAY_OR_OBJECT));
    SINGLESTAGE_SUBTEST("unclosed", R"({ "a"             )",    assert_error(doc["b"], INCOMPLETE_ARRAY_OR_OBJECT));
    SINGLESTAGE_SUBTEST("unclosed", R"({                 )",    assert_error(doc["b"], INCOMPLETE_ARRAY_OR_OBJECT));
    TEST_SUCCEED();
  }
  bool object_lookup_miss_next_error() {
    TEST_START();
    SINGLESTAGE_SUBTEST("missing comma", R"({ "a": 1  "b": 2 })", ([&]() {
      auto obj = doc.get_object();
      return assert_result(obj["a"], int64_t(1)) && assert_error(obj["b"], TAPE_ERROR);
    })());
    TEST_SUCCEED();
  }

#ifdef SIMDJSON_DEVELOPMENT_CHECKS
  bool out_of_order_object_iteration_error() {
    TEST_START();
    auto json = R"([ { "x": 1, "y": 2 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      for (auto element : doc) {
        auto obj = element.get_object();
        for (auto field : obj) { ASSERT_SUCCESS(field); }
        ASSERT_ITERATE_ERROR( obj, OUT_OF_ORDER_ITERATION );
      }
      return true;
    }));
    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      for (auto element : doc) {
        singlestage::object obj;
        ASSERT_SUCCESS( element.get(obj) );
        for (auto field : obj) { ASSERT_SUCCESS(field); }
        ASSERT_ITERATE_ERROR( obj, OUT_OF_ORDER_ITERATION );
      }
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_top_level_object_iteration_error() {
    TEST_START();
    auto json = R"({ "x": 1, "y": 2 })"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      auto obj = doc.get_object();
      for (auto field : obj) { ASSERT_SUCCESS(field); }
      ASSERT_ITERATE_ERROR( obj, OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object obj;
      ASSERT_SUCCESS( doc.get(obj) );
      for (auto field : obj) { ASSERT_SUCCESS(field); }
      ASSERT_ITERATE_ERROR( obj, OUT_OF_ORDER_ITERATION );
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_index_child_error() {
    TEST_START();
    auto json = R"([ { "x": 1, "y": 2 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> obj;
      for (auto element : doc) {
        obj = element.get_object();
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj["x"], OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj["x"], OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> obj;
      for (auto element : doc) {
        obj = element;
        ASSERT_SUCCESS( obj["x"] );
      }
      ASSERT_ERROR( obj["x"], OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        ASSERT_SUCCESS( obj["x"] );
      }
      ASSERT_ERROR( obj["x"], OUT_OF_ORDER_ITERATION );
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_index_sibling_error() {
    TEST_START();
    auto json = R"([ { "x": 0, "y": 2 }, { "x": 1, "y": 4 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element.get_object();
        uint64_t x;
        ASSERT_SUCCESS(obj["x"].get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj["x"].get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::object obj;
        ASSERT_SUCCESS( element.get_object().get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj["x"].get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj["x"].get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element;
        uint64_t x;
        ASSERT_SUCCESS(obj["x"].get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj["x"].get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::value obj;
        ASSERT_SUCCESS( element.get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj["x"].get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj["x"].get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_find_field_child_error() {
    TEST_START();
    auto json = R"([ { "x": 1, "y": 2 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> obj;
      for (auto element : doc) {
        obj = element.get_object();
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj.find_field("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj.find_field("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> obj;
      for (auto element : doc) {
        obj = element;
        ASSERT_SUCCESS( obj.find_field("x") );
      }
      ASSERT_ERROR( obj.find_field("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        ASSERT_SUCCESS( obj.find_field("x") );
      }
      ASSERT_ERROR( obj.find_field("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_find_field_sibling_error() {
    TEST_START();
    auto json = R"([ { "x": 0, "y": 2 }, { "x": 1, "y": 4 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element.get_object();
        uint64_t x;
        ASSERT_SUCCESS(obj.find_field("x").get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::object obj;
        ASSERT_SUCCESS( element.get_object().get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj.find_field("x").get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element;
        uint64_t x;
        ASSERT_SUCCESS(obj.find_field("x").get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::value obj;
        ASSERT_SUCCESS( element.get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj.find_field("x").get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_find_field_unordered_child_error() {
    TEST_START();
    auto json = R"([ { "x": 1, "y": 2 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> obj;
      for (auto element : doc) {
        obj = element.get_object();
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj.find_field_unordered("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        for (auto field : obj) { ASSERT_SUCCESS(field); }
      }
      ASSERT_ERROR( obj.find_field_unordered("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> obj;
      for (auto element : doc) {
        obj = element;
        ASSERT_SUCCESS( obj.find_field_unordered("x") );
      }
      ASSERT_ERROR( obj.find_field_unordered("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value obj;
      for (auto element : doc) {
        ASSERT_SUCCESS( element.get(obj) );
        ASSERT_SUCCESS( obj.find_field_unordered("x") );
      }
      ASSERT_ERROR( obj.find_field_unordered("x"), OUT_OF_ORDER_ITERATION );
      return true;
    }));
    TEST_SUCCEED();
  }

  bool out_of_order_object_find_field_unordered_sibling_error() {
    TEST_START();
    auto json = R"([ { "x": 0, "y": 2 }, { "x": 1, "y": 4 } ])"_padded;
    SUBTEST("simdjson_result<object>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::object> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element.get_object();
        uint64_t x;
        ASSERT_SUCCESS(obj.find_field_unordered("x").get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field_unordered("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("object", test_singlestage_doc(json, [&](auto doc) {
      singlestage::object last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::object obj;
        ASSERT_SUCCESS( element.get_object().get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj.find_field_unordered("x").get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field_unordered("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("simdjson_result<value>", test_singlestage_doc(json, [&](auto doc) {
      simdjson_result<singlestage::value> last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        auto obj = element;
        uint64_t x;
        ASSERT_SUCCESS(obj.find_field_unordered("x").get(x));
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field_unordered("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));

    SUBTEST("value", test_singlestage_doc(json, [&](auto doc) {
      singlestage::value last_obj;
      uint64_t i = 0;
      for (auto element : doc) {
        singlestage::value obj;
        ASSERT_SUCCESS( element.get(obj) );
        uint64_t x;
        ASSERT_SUCCESS( obj.find_field_unordered("x").get(x) );
        ASSERT_EQUAL(x, i);
        if (i > 0) {
          ASSERT_ERROR(last_obj.find_field_unordered("x").get(x), OUT_OF_ORDER_ITERATION);
          break;
        }
        last_obj = obj;
        i++;
      }
      return true;
    }));
    TEST_SUCCEED();
  }
#endif

  bool run() {
    return
           object_iterate_error() &&
           object_iterate_wrong_key_type_error() &&
           object_iterate_unclosed_error() &&
           object_lookup_error() &&
           object_lookup_unclosed_error() &&
           object_lookup_miss_error() &&
           object_lookup_miss_unclosed_error() &&
           object_lookup_miss_wrong_key_type_error() &&
           object_lookup_miss_next_error() &&
#ifdef SIMDJSON_DEVELOPMENT_CHECKS
           out_of_order_object_iteration_error() &&
           out_of_order_top_level_object_iteration_error() &&
           out_of_order_object_index_child_error() &&
           out_of_order_object_index_sibling_error() &&
           out_of_order_object_find_field_child_error() &&
           out_of_order_object_find_field_sibling_error() &&
           out_of_order_object_find_field_unordered_child_error() &&
           out_of_order_object_find_field_unordered_sibling_error() &&
#endif
           true;
  }
} // namespace object_error_tests

int main(int argc, char *argv[]) {
  return test_main(argc, argv, object_error_tests::run);
}