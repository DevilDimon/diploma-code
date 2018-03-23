#pragma once
#include <wdm.h>

#define INITIAL_CAPACITY PAGE_SIZE
#define POOL_TAG 'ceVK'

namespace gene_km_internal {

template <typename T>
class kvector {

private:
	T *data;
	ULONG_PTR size;
	ULONG_PTR capacity;
	bool valid;

	bool increase_capacity(ULONG_PTR extra_size) {
		UNREFERENCED_PARAMETER(extra_size);
		return true;
	}

public:
	kvector() {
		size = 0;
		capacity = INITIAL_CAPACITY;
		data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, capacity, POOL_TAG));
		valid = (data != nullptr);
	}

	bool push_back(const T &value) {
		if ((size + 1) * sizeof(T) > capacity) {
			bool res = increase_capacity(sizeof(T));
			if (!res) {
				return false;
			}
		}

		RtlCopyMemory(data + size, &value, sizeof(value));
		size += 1;
		
		return true;
	}

	T &operator[](ULONG_PTR pos) {
		if (pos >= size || pos < 0) {
			return nullptr;
		}

		return data[pos];
	}

	~kvector() {
		if (data == nullptr) {
			return;
		}

		ExFreePoolWithTag(data, POOL_TAG);

		data = nullptr;
		size = 0;
		capacity = 0;
	}

};

} // namespace gene_km_internal