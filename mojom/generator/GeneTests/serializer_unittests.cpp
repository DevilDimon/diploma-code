#include "pch.h"
#include <utility>

#include "..\gene\gene_embedded_types.h"

namespace gene_test_serialization {

template <typename T>
bool serialize_and_deserialize(T *in, T *out) {
	gene_internal::container c;
	if (!gene_internal::serialize(*in, c)) {
		return false;
	}
	if (!gene_internal::deserialize(c, out)) {
		return false;
	}

	return true;
}

TEST(TestSerialization, IntValues) {
	int test_cases[] = { 0, -1, 1,
		std::numeric_limits<int>::min(),
		std::numeric_limits<int>::max() };

	for (int int_case : test_cases) {
		int in = int_case;
		int out;
		ASSERT_TRUE(serialize_and_deserialize(&in, &out));
		ASSERT_EQ(in, out);
	}
}

TEST(TestSerialization, FloatValues) {
	float test_cases[] = { -0.0, +0.0, -1.0, +1.0,
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::max() };

	for (float float_case : test_cases) {
		float in = float_case;
		float out;
		ASSERT_TRUE(serialize_and_deserialize(&in, &out));
		ASSERT_EQ(in, out);
	}
}

TEST(TestSerialization, DoubleValues) {
	double test_cases[] = { -0.0, +0.0, -1.0, +1.0,
		std::numeric_limits<double>::min(),
		std::numeric_limits<double>::max() };

	for (double double_case : test_cases) {
		double in = double_case;
		double out;
		ASSERT_TRUE(serialize_and_deserialize(&in, &out));
		ASSERT_EQ(in, out);
	}
}

TEST(TestSerialization, StringValues) {
	std::string test_cases[] = { "", "ascii", "\xf0\x9f\x8e\x86" };

	for (std::string string_case : test_cases) {
		std::string in = string_case;
		std::string out;
		ASSERT_TRUE(serialize_and_deserialize(&in, &out));
		ASSERT_EQ(in, out);
	}

}

} // namespace gene_test_serialization