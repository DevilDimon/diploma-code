#pragma once
#ifndef GENE_KERNEL_MODE
#include <vector>
#include <string>
#include <cstdint>
#else
#include "KVector.h"
#include "KString.h"
#include "kmstdint.h"
#define _NEW_DELETE_OPERATORS_
#include "libc.h"
#endif