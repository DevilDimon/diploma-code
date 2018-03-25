#include <wdm.h>
#include "gene_km_basics.h"

namespace gene_km_internal {

bool send_message_internal(const kcontainer &c) {
	UNREFERENCED_PARAMETER(c);
	// TODO: actual send
	return true;
}

bool receive_message_internal(kcontainer *c) {
	UNREFERENCED_PARAMETER(c);
	return true;
}

}