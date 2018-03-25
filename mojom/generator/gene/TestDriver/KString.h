#pragma once

namespace std {

class string {
private:
	char *str;
	size_t len;
public:
	using size_type = size_t;
	string();
	string(const char *chars);
	string(const string &other);
	string(string&& other);
	string &operator=(const string &other);
	string &append(size_type count, char ch);
	size_type size() const;
	bool operator==(const string &other) const;
	bool operator!=(const string &other) const;
	bool operator<(const string &other) const;
	bool operator>(const string &other) const;
	char *begin();
	const char *begin() const;
	char *end();
	const char *end() const;
	~string();

};

} // namespace std