#include "KString.h"
#include <wdm.h>

#define POOL_TAG 'rtsK'

namespace std {

string::string() {
	str = nullptr;
	len = 0;
}

string::string(const char *chars) {
	size_t length = strlen(chars);
	str = static_cast<char *>(ExAllocatePoolWithTag(PagedPool, length + 1, POOL_TAG));
	RtlCopyMemory(str, chars, length);
	len = length;
}

string::string(const string &other) {
	if (other.str) {
		str = static_cast<char *>(ExAllocatePoolWithTag(PagedPool, other.len, POOL_TAG));
		RtlCopyMemory(str, other.str, other.len);
	}
	len = other.len;
}

string::string(string &&other) {
	str = other.str;
	len = other.len;
	other.str = nullptr;
	other.len = 0;
}

string &string::operator=(const string &other) {
	if (other.str) {
		str = static_cast<char *>(ExAllocatePoolWithTag(PagedPool, other.len, POOL_TAG));
		RtlCopyMemory(str, other.str, other.len);
	}
	len = other.len;
	return *this;
}

string &string::append(string::size_type count, char ch) {
	if (count <= 0) {
		return *this;
	}

	size_t new_len = len + count;
	char *new_str = static_cast<char *>(ExAllocatePoolWithTag(PagedPool, new_len + 1, POOL_TAG));
	RtlCopyMemory(new_str, str, len);
	for (string::size_type i = 0; i < count; ++i) {
		new_str[len + i] = ch;
	}
	new_str[len + count] = '\0';
	
	if (str) {
		ExFreePoolWithTag(str, POOL_TAG);
	}
	str = new_str;
	len = len + count;

	return *this;
}

string::size_type string::size() const {
	return len;
}

bool string::operator==(const string &other) const {
	return strncmp(str, other.str, len) == 0;
}

bool string::operator!=(const string &other) const {
	return !operator==(other);
}

bool string::operator<(const string &other) const {
	return strncmp(str, other.str, len) < 0;
}

bool string::operator>(const string &other) const {
	return strncmp(str, other.str, len) > 0;
}

char *string::begin() {
	return str;
}

const char *string::begin() const {
	return str;
}

char *string::end() {
	return str + len;
}

const char *string::end() const {
	return str + len;
}

string::~string() {
	if (str) {
		ExFreePoolWithTag(str, POOL_TAG);
		len = 0;
	}
}

} // namespace std