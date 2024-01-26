#pragma once
#include "enums/function_name.h"
#include "utility.h"

int check_operation_result(FUNC func, const int err_code, hid_device *handle, uint8_t *r = nullptr);