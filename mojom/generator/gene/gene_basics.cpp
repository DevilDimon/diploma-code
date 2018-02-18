#include "gene_basics.h"

namespace gene_internal {
    container cc;

    bool send_message_internal(const container &c) {
        cc = c;
        return true;
    }

    bool receive_message_internal(container *c) {
        *c = cc;
        return true;
    }
}
