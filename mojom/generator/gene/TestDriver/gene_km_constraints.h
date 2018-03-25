#pragma once
#include "KVector.h"

#include "gene_km_basics.h"

namespace gene_km_internal {

// vector constraints

template <typename T> struct size_equals_constraint<kvector<T>> : constraint<kvector<T>> {
	UINT32 value;
	explicit size_equals_constraint(UINT32 value) : value(value) {}
	bool check(const kvector<T> &v) const {
		return v.size() == value;
	}
};

template <typename T> struct size_not_equals_constraint<kvector<T>> : constraint<kvector<T>> {
	UINT32 value;
	explicit size_not_equals_constraint(UINT32 value) : value(value) {}
	bool check(const kvector<T> &v) const {
		return v.size() != value;
	}
};

template <typename T> struct size_lesser_constraint<kvector<T>> : constraint<kvector<T>> {
	UINT32 value;
	explicit size_lesser_constraint(UINT32 value) : value(value) {}
	bool check(const kvector<T> &v) const {
		return v.size() < value;
	}
};

template <typename T> struct size_greater_constraint<kvector<T>> : constraint<kvector<T>> {
	UINT32 value;
	explicit size_greater_constraint(UINT32 value) : value(value) {}
	bool check(const kvector<T> &v) const {
		return v.size() > value;
	}
};

// string constraints

/*
template <> struct size_equals_constraint<std::string> : constraint<std::string> {
	UINT32 value;
	explicit size_equals_constraint(UINT32 value) : value(value) {}
	bool check(const std::string &v) const override {
		return v.size() == value;
	}
};

template <> struct size_not_equals_constraint<std::string> : constraint<std::string> {
	UINT32 value;
	explicit size_not_equals_constraint(UINT32 value) : value(value) {}
	bool check(const std::string &v) const override {
		return v.size() != value;
	}
};

template <> struct size_lesser_constraint<std::string> : constraint<std::string> {
	UINT32 value;
	explicit size_lesser_constraint(UINT32 value) : value(value) {}
	bool check(const std::string &v) const override {
		return v.size() < value;
	}
};

template <> struct size_greater_constraint<std::string> : constraint<std::string> {
	UINT32 value;
	explicit size_greater_constraint(UINT32 value) : value(value) {}
	bool check(const std::string &v) const override {
		return v.size() > value;
	}
};

*/
}  // namespace gene_km_internal