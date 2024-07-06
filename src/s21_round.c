#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_other_error_code code = other_ERROR;
  s21_decimal tmp;
  make_zero_decimal(&tmp);
  for (int i = 0; i < 4; i++) tmp.bits[i] = value.bits[i];
  unsigned int number = 0, plus = 0;
  int degree = get_degree(value), point = 0;
  for (int i = 0; i < degree; i++) {
    number = divide_by_ten(&tmp);
    plus += number;
  }
  plus -= number;
  if ((number == 5 && plus > 0) || number > 5) point = 1;
  help_round(&value, point);
  code = other_OK;
  for (int i = 0; i < 4; i++) result->bits[i] = value.bits[i];
  return code;
}