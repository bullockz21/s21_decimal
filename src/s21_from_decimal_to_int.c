#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  s21_other_error_code code = other_OK;
  if (!dst) {
    code = other_ERROR;
  } else {
    s21_truncate(src, &src);
    if (src.bits[2] != 0 || src.bits[1] != 0) {
      code = other_ERROR;
    } else if (src.bits[0] >= INT_MAX) {
      code = other_ERROR;
    } else {
      *dst = src.bits[0];
      if (get_bit(src, S21_SIGN_INDEX) == 1) {
        *dst *= -1;
      }
    }
  }
  return code;
}