#pragma once
#include <wdm.h>

#define INITIAL_CAPACITY PAGE_SIZE
#define POOL_TAG 'ceVK'

namespace gene_km_internal {

template <typename T>
class kvector {

private:
	T *_data;
	ULONG_PTR _size;
	ULONG_PTR capacity;
	bool valid;

	ULONG_PTR align_to_page_size(ULONG_PTR length) {
		return (length + (PAGE_SIZE - 1)) & -PAGE_SIZE;
	}

	bool increase_capacity(ULONG_PTR num_elements) {
		T *new_data = nullptr;
		capacity = align_to_page_size((_size + num_elements) * sizeof(T));
		new_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, capacity, POOL_TAG));
		if (new_data == nullptr) {
			capacity -= num_elements * sizeof(T);
			return false;
		}

		RtlCopyMemory(new_data, _data, _size * sizeof(T));
		ExFreePoolWithTag(_data, POOL_TAG);
		_data = new_data;

		return true;
	}

public:
	kvector() {
		_size = 0;
		capacity = INITIAL_CAPACITY;
		_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, capacity, POOL_TAG));
		valid = (_data != nullptr);
	}

	bool push_back(const T &value) {
		if ((_size + 1) * sizeof(T) > capacity) {
			bool res = increase_capacity(1);
			if (!res) {
				return false;
			}
		}

		RtlCopyMemory(_data + _size, &value, sizeof(value));
		_size += 1;
		
		return true;
	}

	bool erase(ULONG_PTR first, ULONG_PTR last) {
		if (first >= last) {
			return false;
		}

		ULONG_PTR erased_count = last - first;

		if (align_to_page_size((_size - erased_count) * sizeof(T)) < capacity) {
			T *new_data = nullptr;
			capacity = align_to_page_size((_size - erased_count) * sizeof(T));
			new_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, capacity, POOL_TAG));
			if (new_data == nullptr) {
				capacity += erased_count * sizeof(T);
				return false;
			}

			RtlCopyMemory(new_data, _data, first * sizeof(T));
			RtlCopyMemory(new_data + first, _data + last, (_size - last) * sizeof(T));
			ExFreePoolWithTag(_data, POOL_TAG);
			_data = new_data;
		}
		else {
			RtlMoveMemory(_data + first, _data + last, (_size - erased_count) * sizeof(T));
		}

		_size -= erased_count;
		return true;
	}

	bool at(ULONG_PTR pos, T *value) const {
		if (pos >= _size || pos < 0) {
			return false;
		}

		*value = _data[pos];
		return true;
	}

	bool set(ULONG_PTR pos, const T &value) {
		if (pos >= _size || pos < 0) {
			return false;
		}

		_data[pos] = value;
		return true;
	}

	ULONG_PTR size() const {
		return _size;
	}

	T *data() {
		return _data;
	}

	const T *data() const {
		return _data;
	}

	~kvector() {
		if (_data == nullptr) {
			return;
		}

		ExFreePoolWithTag(_data, POOL_TAG);

		_data = nullptr;
		_size = 0;
		capacity = 0;
	}

};

} // namespace gene_km_internal