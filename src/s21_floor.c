#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  s21_other_error_code code = other_OK;
  s21_decimal tmp;
  for (int i = 0; i < 4; i++) tmp.bits[i] = value.bits[i];
  unsigned int number = 0;
  int sign = get_bit(value, S21_SIGN_INDEX);
  int degree = get_degree(value), point = 0;
  if (sign) {
    for (int i = 0; i < degree && !number; i++) {
      number += divide_by_ten(&tmp);
    }
    point = 0;
    if (number > 0) point = 1;
    help_round(&value, point);
    for (int i = 0; i < 4; i++) result->bits[i] = value.bits[i];
  } else {
    s21_truncate(value, result);
  }
  return code;
}