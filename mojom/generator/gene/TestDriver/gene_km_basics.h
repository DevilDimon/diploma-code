#pragma once
#include "KVector.h"

namespace gene_km_internal {

using kcontainer = kvector<UINT8>;

bool send_message_internal(const kcontainer &c);
bool receive_message_internal(kcontainer *c);

template <typename T> struct serializer {
	bool operator()(const T &v, kcontainer &c);
	bool operator()(kcontainer &c, T *v);
};

template <typename T> bool serialize(const T &v, kcontainer &c) {
	serializer<T> serializer;
	return serializer(v, c);
}

template <typename T> bool deserialize(kcontainer &c, T *v) {
	serializer<T> serializer;
	return serializer(c, v);
}

template <typename T> struct constraint {
	virtual bool check(const T &v) const = 0;
};

// value constraints

template <typename T> struct value_equals_constraint : constraint<T> {
	T value;
	explicit value_equals_constraint(T value) : value(value) {}
	bool check(const T &v) const {
		return v == value;
	}
};

template <typename T> struct value_not_equals_constraint : constraint<T> {
	T value;
	explicit value_not_equals_constraint(T value) : value(value) {}
	bool check(const T &v) const {
		return v != value;
	}
};

template <typename T> struct value_lesser_constraint : constraint<T> {
	T value;
	explicit value_lesser_constraint(T value) : value(value) {}
	bool check(const T &v) const {
		return v < value;
	}
};

template <typename T> struct value_greater_constraint : constraint<T> {
	T value;
	explicit value_greater_constraint(T value) : value(value) {}
	bool check(const T &v) const {
		return v> value;
	}
};

// size constraints

template <typename T> struct size_equals_constraint : constraint<T> {
	UINT32 value;
	explicit size_equals_constraint(UINT32 value) : value(value) {}
	bool check(const T &v) const {
		return sizeof(T) == value;
	}
};

template <typename T> struct size_not_equals_constraint : constraint<T> {
	UINT32 value;
	explicit size_not_equals_constraint(UINT32 value) : value(value) {}
	bool check(const T &v) const {
		return sizeof(T) != value;
	}
};

template <typename T> struct size_lesser_constraint : constraint<T> {
	UINT32 value;
	explicit size_lesser_constraint(UINT32 value) : value(value) {}
	bool check(const T &v) const {
		return sizeof(T) < value;
	}
};

template <typename T> struct size_greater_constraint : constraint<T> {
	UINT32 value;
	explicit size_greater_constraint(UINT32 value) : value(value) {}
	bool check(const T &v) const {
		return sizeof(T) > value;
	}
};

template <typename T> struct compound_constraint : constraint<T> {
	const constraint<T> &constraint_one;
	const constraint<T> &constraint_two;

	compound_constraint(const constraint<T> &constraint_one, const constraint<T> &constraint_two) :
		constraint_one(constraint_one),
		constraint_two(constraint_two) {}

	bool check(const T &v) const {
		return constraint_one.check(v) || constraint_two.check(v);
	}
};


} // namespace gene_km_internal