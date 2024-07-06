#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  s21_other_error_code code = other_ERROR;
  if (result) {
    result->bits[3] = value.bits[3];
    result->bits[3] ^= (1 << 31);
    for (int i = 0; i < 3; i++) {
      result->bits[i] = value.bits[i];
    }
    code = other_OK;
  }
  return code;
}