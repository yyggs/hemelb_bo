// -*- mode: C++ -*-
// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include <type_traits>

#include <catch2/catch.hpp>

#include "io/writers/xdr/XdrMemReader.h"
// This header is generated by xdr_gen.py
#include "tests/io/xdr_test_data.h"

namespace hemelb
{
  namespace tests
  {
    namespace {
      template<typename T>
      void TestBasic(const std::vector<T>& expected_values, const std::vector<char>& buffer) {
	// Figure out sizes and counts
	constexpr auto sz = sizeof(T);
	static_assert(sz > 0U && sz <= 8U,
		      "Only works on 8--64 bit types");
	constexpr auto type_bits = sz*8U;
	// XDR works on 32 bit words
	constexpr auto coded_words = (sz - 1U)/4U + 1U;
	constexpr auto coded_bytes = coded_words * 4U;
	// Need a value for each bit being on + zero
	constexpr auto n_vals = type_bits + 1U;
	// Buffers for the encoded data
	constexpr auto buf_size = n_vals * coded_bytes;
	REQUIRE(expected_values.size() == n_vals);
	REQUIRE(buffer.size() == buf_size);
	// Make the decoder under test
	auto our_coder = hemelb::io::writers::xdr::XdrMemReader(buffer.data(), buffer.size());

	// Decode values and check they are equal
	for (auto i = 0U; i < n_vals; ++i) {
	  T our_val;
	  our_coder.read(our_val);
	  REQUIRE(expected_values[i] == our_val);
	}
      }
    }

    TEMPLATE_TEST_CASE("XdrReader works for integers", "", int32_t, uint32_t, int64_t, uint64_t) {
      static_assert(std::is_integral<TestType>::value, "only works on (u)ints");
      auto&& values = test_data<TestType>::unpacked();
      auto&& buffer = test_data<TestType>::packed();

      TestBasic<TestType>(values, buffer);
    }

    TEMPLATE_TEST_CASE("XdrReader works for floating types", "", float, double) {
      static_assert(std::is_floating_point<TestType>::value, "Floats only please!");

      auto&& values = test_data<TestType>::unpacked();
      auto&& buffer = test_data<TestType>::packed();
	  
      TestBasic(values, buffer);
    }

    TEST_CASE("XdrReader works for strings") {
      using UPC = std::unique_ptr<char[]>;
      auto make_ones = [](size_t n) -> UPC {
	// Fill with binary ones to trigger failure if we're not
	// writing enough zeros
	auto ans = UPC(new char[n]);
	std::fill(ans.get(), ans.get() + n, ~'\0');
	return ans;
      };

      // XDR strings are serialised as length, data (0-padded to a word)
      auto coded_length = [](const std::string& s) {
	return s.size() ? 4U * (2U + (s.size() - 1U) / 4U) : 4U;
      };

      auto&& values = test_data<std::string>::unpacked();
      auto&& buffer = test_data<std::string>::packed();

      std::size_t total_length = 0;
      std::for_each(
	values.begin(), values.end(),
	[&](const std::string& v) {
	  total_length += coded_length(v);
	});
      REQUIRE(buffer.size() == total_length);

      auto our_coder = io::writers::xdr::XdrMemReader(buffer.data(), buffer.size());

      // Decode values and check they are equal
      for (auto& expected_val: values) {
	std::string our_val;
	our_coder.read(our_val);
	REQUIRE(expected_val == our_val);
      }
    }

  }
}
