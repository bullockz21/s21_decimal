#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_other_error_code code = other_OK;
  if (abs(src) > S21_MAX_DEC) {
    code = other_ERROR;
  } else if (src == S21_NAN || src == S21_INF) {
    code = other_ERROR;
  } else {
    make_zero_decimal(dst);
    if (src < 0) {
      src *= -1;
      dst->bits[3] = 1 << 31;
    }
    dst->bits[0] = src;
  }
  return code;
}