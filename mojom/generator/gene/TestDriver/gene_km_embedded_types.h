#pragma once

#include "gene_km_constraints.h"

namespace gene_km_internal {

template <typename T> static bool raw_serialize(const T &v, kcontainer &c) {
	T temp = v;
	PUINT8 arr = reinterpret_cast<PUINT8>(&temp);
	for (int i = 0; i < sizeof(temp); ++i) {
		c.push_back(arr[i]);
	}
	return true;
}

template <typename T> static bool raw_deserialize(kcontainer &c, T *v) {
	UINT8 arr[sizeof(T)];
	for (int i = 0; i < sizeof(T); ++i) {
		c.at(i, &arr[i]);
	}
	c.erase(0, sizeof(T));
	T *res = reinterpret_cast<T *>(arr);
	*v = *res;
	return true;
}

// int types
template <> struct serializer<INT8> {
	bool operator()(const INT8 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PINT8 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<INT16> {
	bool operator()(const INT16 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PINT16 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<INT32> {
	bool operator()(const INT32 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PINT32 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<INT64> {
	bool operator()(const INT64 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PINT64 v) const {
		return raw_deserialize(c, v);
	}
};

// unsigned int types
template <> struct serializer<UINT8> {
	bool operator()(const UINT8 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PUINT8 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<UINT16> {
	bool operator()(const UINT16 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PUINT16 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<UINT32> {
	bool operator()(const UINT32 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PUINT32 v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<UINT64> {
	bool operator()(const UINT64 &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, PUINT64 v) const {
		return raw_deserialize(c, v);
	}
};

// floating types
template <> struct serializer<float> {
	bool operator()(const float &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, float *v) const {
		return raw_deserialize(c, v);
	}
};

template <> struct serializer<DOUBLE> {
	bool operator()(const DOUBLE &v, kcontainer &c) {
		return raw_serialize(v, c);
	}
	bool operator()(kcontainer &c, DOUBLE *v) const {
		return raw_deserialize(c, v);
	}
};

// string types
/*
template <> struct serializer<std::string> {
	bool operator()(const std::string &v, container &c) {
		if (!raw_serialize(v.size(), c)) {
			return false;
		}

		for (char it : v) {
			c.push_back(static_cast<uINT8 &&>(it));
		}
		return true;
	}
	bool operator()(container &c, std::string *v) const {
		std::string::size_type size;
		if (!raw_deserialize(c, &size)) {
			return false;
		}
		*v = std::string();
		auto it = c.begin();
		while (it != c.begin() + size) {
			v->append({ static_cast<char>(*it) });
			++it;
		}
		c.erase(c.begin(), it);
		return true;
	}
};
*/

// container types
template <typename T> struct serializer<kvector<T>> {
	bool operator()(const kvector<T> &v, kcontainer &c) {
		if (!raw_serialize(v.size(), c)) {
			return false;
		}

		for (const auto &item : v) {
			if (!serialize(item, c)) {
				return false;
			}
		}

		return true;
	}
	bool operator()(kcontainer &c, kvector<T> *v) const {
		ULONG_PTR size;
		if (!raw_deserialize(c, &size)) {
			return false;
		}

		*v = kvector<T>();
		for (int i = 0; i < size; ++i) {
			T item;
			if (!deserialize(c, &item)) {
				return false;
			}
			v->push_back(item);
		}

		return true;
	}
};

}  // namespace gene_internal
