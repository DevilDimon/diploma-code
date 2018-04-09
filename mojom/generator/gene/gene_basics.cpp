#include "gene_basics.h"

namespace gene_internal {

bool is_error(const container &c) {
	if (c.size() < sizeof(uint64_t)) {
		return false;
	}

	uint8_t arr[sizeof(uint64_t)];
	for (int i = 0; i < sizeof(uint64_t); ++i) {
		arr[i] = c[i];
	}
	return *reinterpret_cast<uint64_t *>(arr) == gene_error_code;
}

} // namespace gene_internal