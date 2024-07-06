#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_arithmetic_error_code code = arithmetic_OK;
  if (result != NULL) {
    make_zero_decimal(result);
    int degree = 0;
    s21_big_decimal val_1 = {0}, val_2 = {0}, res = {0};
    from_decimal_to_big_decimal(value_1, &val_1);
    from_decimal_to_big_decimal(value_2, &val_2);
    val_1.bits[5] = 0;
    val_2.bits[5] = 0;
    if (get_sign(value_1) != get_sign(value_2)) set_sign(result, 1);
    degree = get_degree(value_1) + get_degree(value_2);
    code = simple_mul(val_1, val_2, &res);
    degree = div_degree(&res, degree);
    if (degree >= 0) {
      for (int i = 0; i < 3; i++) result->bits[i] = res.bits[i];
      if (!check_empty(*result)) set_degree(result, degree);
    } else {
      code = arithmetic_BIG;
    }
    if (code == arithmetic_BIG && get_sign(*result)) code = arithmetic_SMALL;
    if (code) make_zero_decimal(result);
  } else
    code = arithmetic_BIG;
  return code;
}

int simple_mul(s21_big_decimal value_1, s21_big_decimal value_2,
               s21_big_decimal *result) {
  s21_arithmetic_error_code code = arithmetic_OK;
  int count = 0;
  make_zero_big_decimal(result);
  for (int i = 0; i <= S21_BIG_MANTISSA_LAST_INDEX; i++) {
    if (get_big_bit(value_2, i)) {
      value_1 = shift_left_big_decimal(value_1, i - count, &code);
      simple_add(value_1, *result, result);
      count = i;
    }
  }
  return code;
}