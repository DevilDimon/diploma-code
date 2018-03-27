#pragma once
#include <wdm.h>

#define INITIAL_CAPACITY PAGE_SIZE
#define POOL_TAG 'ceVK'

namespace std {

template <typename T> class vector {

private:
	T *_data;
	ULONG_PTR _size;
	ULONG_PTR _capacity;

	ULONG_PTR align_to_page_size(ULONG_PTR length) {
		return (length + (PAGE_SIZE - 1)) & -PAGE_SIZE;
	}

	bool increase_capacity(ULONG_PTR num_elements) {
		T *new_data = nullptr;
		_capacity = align_to_page_size((_size + num_elements) * sizeof(T));
		new_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, _capacity, POOL_TAG));
		if (new_data == nullptr) {
			_capacity -= num_elements * sizeof(T);
			return false;
		}

		RtlCopyMemory(new_data, _data, _size * sizeof(T));
		ExFreePoolWithTag(_data, POOL_TAG);
		_data = new_data;

		return true;
	}

public:
	using size_type = ULONG_PTR;

	class iterator {
	private:
		const vector<T> *vec;
		ULONG_PTR pos;
		friend class vector;
	public:
		iterator(const vector<T> *vec, ULONG_PTR pos) : vec(vec), pos(pos) {}

		iterator(const iterator &other) : vec(other.vec), pos(other.pos) {}

		bool operator==(const iterator &other) const {
			return pos == other.pos;
		}

		bool operator!=(const iterator &other) const {
			return !operator==(other);
		}

		const T & operator*() const {
			return vec->operator[](pos);
		}

		iterator &operator++() {
			++pos;
			return *this;
		}

		iterator operator++(int) {
			iterator old = *this;
			++pos;
			return old;
		}

		iterator operator+(size_t shift) {
			iterator old = *this;
			old.pos += shift;
			return old;
		}
	};

	explicit vector() {
		_size = 0;
		_capacity = INITIAL_CAPACITY;
		_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, _capacity, POOL_TAG));
	}

	vector<T> &operator=(const vector<T> &other) {
		if (other._data) {
			_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, other._capacity, POOL_TAG));
			RtlCopyMemory(_data, other._data, other._capacity);
		}

		_capacity = other._capacity;
		_size = other._size;
		return *this;
	}

	bool push_back(const T &value) {
		if ((_size + 1) * sizeof(T) > _capacity) {
			bool res = increase_capacity(1);
			if (!res) {
				return false;
			}
		}

		_data[_size] = value;
		_size += 1;
		
		return true;
	}

	iterator erase(iterator first, iterator last) {
		if (first.pos >= last.pos) {
			return last;
		}

		ULONG_PTR erased_count = last.pos - first.pos;

		if (align_to_page_size((_size - erased_count) * sizeof(T)) < _capacity) {
			T *new_data = nullptr;
			_capacity = align_to_page_size((_size - erased_count) * sizeof(T));
			new_data = static_cast<T *>(ExAllocatePoolWithTag(PagedPool, _capacity, POOL_TAG));
			if (new_data == nullptr) {
				_capacity += erased_count * sizeof(T);
				return last;
			}

			RtlCopyMemory(new_data, _data, first.pos * sizeof(T));
			RtlCopyMemory(new_data + first.pos, _data + last.pos, (_size - last.pos) * sizeof(T));
			ExFreePoolWithTag(_data, POOL_TAG);
			_data = new_data;
		}
		else {
			RtlMoveMemory(_data + first.pos, _data + last.pos, (_size - erased_count) * sizeof(T));
		}

		_size -= erased_count;
		return first + 1;
	}

	iterator begin() const {
		return iterator(this, 0);
	}

	iterator end() const {
		return iterator(this, _size);
	}

	const T &operator[](ULONG_PTR pos) const {
		return _data[pos];
	}

	T &operator[](ULONG_PTR pos) {
		return _data[pos];
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

	~vector() {
		if (_data == nullptr) {
			return;
		}

		ExFreePoolWithTag(_data, POOL_TAG);

		_data = nullptr;
		_size = 0;
		_capacity = 0;
	}

};

} // namespace std