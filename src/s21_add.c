#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_arithmetic_error_code code = arithmetic_OK;
  if (result != NULL) {
    int get = 0, sign_res = 0;
    make_zero_decimal(result);
    if (get_sign(value_1) && get_sign(value_2)) {
      sign_res = 1;
    }
    if (get_sign(value_1) != get_sign(value_2)) {
      int sign = 0;
      sign = get_sign(value_1);
      set_bit(&value_1, S21_SIGN_INDEX, 0);
      set_bit(&value_2, S21_SIGN_INDEX, 0);
      code = sign ? s21_sub(value_2, value_1, result)
                  : s21_sub(value_1, value_2, result);
    } else {
      s21_big_decimal val_1 = {0}, val_2 = {0}, res = {0};
      int degree = 0;
      from_decimal_to_big_decimal(value_1, &val_1);
      from_decimal_to_big_decimal(value_2, &val_2);
      val_1.bits[5] = 0;
      val_2.bits[5] = 0;
      int diff = get_degree(value_1) - get_degree(value_2);
      if (diff > 0) {
        get = get_degree(value_1);
        set_degree(&value_2, get);
      } else {
        get = get_degree(value_2);
        set_degree(&value_1, get);
      }
      degree_aligment(&val_1, &val_2, diff);
      simple_add(val_1, val_2, &res);
      degree = div_degree(&res, get_degree(value_1));
      if (degree >= 0) {
        for (int i = 0; i < 3; i++) result->bits[i] = res.bits[i];
        set_degree(result, degree);
      } else {
        code = 1;
      }
    }
    if (code == arithmetic_OK) {
      if (check_empty(*result))
        result->bits[3] = 0;
      else
        sign_res == 1 ? set_sign(result, 1) : 0;
    }

    if (code == arithmetic_BIG && get_sign(*result)) code = arithmetic_SMALL;
    if (code) make_zero_decimal(result);
  } else
    code = arithmetic_BIG;

  return code;
}

void simple_add(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result) {
  s21_big_decimal res;
  make_zero_big_decimal(&res);
  int tmp = 0, sum = 0;
  for (int index = 0; index <= S21_BIG_MANTISSA_LAST_INDEX; index++) {
    sum = get_big_bit(value_1, index) + get_big_bit(value_2, index) + tmp;
    tmp = sum / 2;
    set_big_bit(&res, index, sum % 2);
  }
  for (int i = 0; i < 6; i++) result->bits[i] = res.bits[i];
}