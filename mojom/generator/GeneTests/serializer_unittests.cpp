#include "pch.h"
#include <utility>
#include <vector>

#include "..\gene\gene_embedded_types.h"

struct test_struct1 {
	struct test_struct2 {
		int int_field = 1;
		inline bool operator==(const test_struct2 &rhs) const {
			return int_field == rhs.int_field;
		}
	};

	test_struct2 struct_field = test_struct2();

	inline bool operator==(const test_struct1 &rhs) const {
		return struct_field == rhs.struct_field;
	}
};

struct ultimate_test_struct {
	int int_field = 1;
	float float_field = 1.f;
	double double_field = 1.0;
	std::vector<test_struct1> vec_field = {
		{{1}}, {{123}}, {{112233}}
	};
	std::string str_field = "ultra-rigorous test";
	test_struct1::test_struct2 struct_field = { 0x2018 };

	inline bool operator==(const ultimate_test_struct &rhs) const {
		return
			int_field == rhs.int_field &&
			float_field == rhs.float_field &&
			double_field == rhs.double_field &&
			vec_field == rhs.vec_field &&
			str_field == rhs.str_field &&
			struct_field == rhs.struct_field;
	}
};

namespace gene_internal {

template <> struct serializer<test_struct1::test_struct2> {
	bool operator()(const test_struct1::test_struct2 &v, container &c) {
		return
			serialize(v.int_field, c);
	}
	bool operator()(container &c, test_struct1::test_struct2 *v) {
		if (!v)
			return false;

		return
			deserialize(c, &v->int_field);
	}
};

template <> struct serializer<test_struct1> {
	bool operator()(const test_struct1 &v, container &c) {
		return
			serialize(v.struct_field, c);
	}
	bool operator()(container &c, test_struct1 *v) {
		if (!v)
			return false;

		return
			deserialize(c, &v->struct_field);
	}
};

template <> struct serializer<ultimate_test_struct> {
	bool operator()(const ultimate_test_struct &v, container &c) {
		return
			serialize(v.int_field, c) &&
			serialize(v.float_field, c) &&
			serialize(v.double_field, c) &&
			serialize(v.vec_field, c) &&
			serialize(v.str_field, c) &&
			serialize(v.struct_field, c);
	}

	bool operator()(container &c, ultimate_test_struct *v) {
		if (!v)
			return false;

		return
			deserialize(c, &v->int_field) &&
			deserialize(c, &v->float_field) &&
			deserialize(c, &v->double_field) &&
			deserialize(c, &v->vec_field) &&
			deserialize(c, &v->str_field) &&
			deserialize(c, &v->struct_field);
	}
};

}

namespace gene_test_serialization {

template <typename T>
bool serialize_and_deserialize(const T &in, T *out) {
	gene_internal::container c;
	
	if (!gene_internal::serialize(in, c)) {
		return false;
	}
	if (!gene_internal::deserialize(c, out)) {
		return false;
	}

	return true;
}

TEST(TestSerialization, NullValues) {
	int val = 0;
	int *null = nullptr;
	ASSERT_FALSE(serialize_and_deserialize(val, null));

	std::string str_val = "hello";
	std::string *null_str = nullptr;
	ASSERT_FALSE(serialize_and_deserialize(str_val, null_str));

	std::vector<int> vec_val = {};
	std::vector<int> *null_vec = nullptr;
	ASSERT_FALSE(serialize_and_deserialize(vec_val, null_vec));
}

TEST(TestSerialization, IntValues) {
	int test_cases[] = { 0, -1, 1,
		std::numeric_limits<int>::min(),
		std::numeric_limits<int>::max() };

	for (int int_case : test_cases) {
		int out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, Int8Values) {
	int8_t test_cases[] = { 0, -1, 1,
		std::numeric_limits<int8_t>::min(),
		std::numeric_limits<int8_t>::max() };

	for (auto int_case : test_cases) {
		int8_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, Int16Values) {
	int16_t test_cases[] = { 0, -1, 1,
		std::numeric_limits<int16_t>::min(),
		std::numeric_limits<int16_t>::max() };

	for (auto int_case : test_cases) {
		int16_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, Int32Values) {
	int32_t test_cases[] = { 0, -1, 1,
		std::numeric_limits<int32_t>::min(),
		std::numeric_limits<int32_t>::max() };

	for (auto int_case : test_cases) {
		int32_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, Int64Values) {
	int64_t test_cases[] = { 0, -1, 1,
		std::numeric_limits<int64_t>::min(),
		std::numeric_limits<int64_t>::max() };

	for (auto int_case : test_cases) {
		int64_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, UInt8Values) {
	uint8_t test_cases[] = { 0, 1,
		std::numeric_limits<uint8_t>::max() };

	for (auto int_case : test_cases) {
		uint8_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, UInt16Values) {
	uint16_t test_cases[] = { 0, 1,
		std::numeric_limits<uint16_t>::max() };

	for (auto int_case : test_cases) {
		uint16_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, UInt32Values) {
	uint32_t test_cases[] = { 0, 1,
		std::numeric_limits<uint32_t>::max() };

	for (auto int_case : test_cases) {
		uint32_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, UInt64Values) {
	uint64_t test_cases[] = { 0, 1,
		std::numeric_limits<uint64_t>::max() };

	for (auto int_case : test_cases) {
		uint64_t out;
		ASSERT_TRUE(serialize_and_deserialize(int_case, &out));
		ASSERT_EQ(int_case, out);
	}
}

TEST(TestSerialization, FloatValues) {
	float test_cases[] = { -0.0f, +0.0f, -1.0f, +1.0f,
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::max() };

	for (float float_case : test_cases) {
		float out;
		ASSERT_TRUE(serialize_and_deserialize(float_case, &out));
		ASSERT_EQ(float_case, out);
	}
}

TEST(TestSerialization, DoubleValues) {
	double test_cases[] = { -0.0, +0.0, -1.0, +1.0,
		std::numeric_limits<double>::min(),
		std::numeric_limits<double>::max() };

	for (double double_case : test_cases) {
		double out;
		ASSERT_TRUE(serialize_and_deserialize(double_case, &out));
		ASSERT_EQ(double_case, out);
	}
}

TEST(TestSerialization, PrimitiveVectorValues) {
	std::vector<int> test_cases[] = {
		{}, {1}, {1, 2, 3},
	};

	for (auto vector_case : test_cases) {
		std::vector<int> out;
		ASSERT_TRUE(serialize_and_deserialize(vector_case, &out));
		ASSERT_EQ(vector_case, out);
	}
}

TEST(TestSerialization, StructuredVectorValues) {
	test_struct1 in = { {2} };
	test_struct1 out;
	ASSERT_TRUE(serialize_and_deserialize(in, &out));
	ASSERT_EQ(in, out);
}

TEST(TestSerialization, StringValues) {
	std::string test_cases[] = { "", "ascii", "\xf0\x9f\x8e\x86" };

	for (std::string string_case : test_cases) {
		std::string out;
		ASSERT_TRUE(serialize_and_deserialize(string_case, &out));
		ASSERT_EQ(string_case, out);
	}
}

TEST(TestSerialization, UltimateTest) {
	ultimate_test_struct in = {
		2, 2.f, 2.0, {{{2}}}, "mega rigorous testing", {{0x2019}}
	};
	ultimate_test_struct out;
	ASSERT_TRUE(serialize_and_deserialize(in, &out));
	ASSERT_EQ(in, out);
}

} // namespace gene_test_serialization