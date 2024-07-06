#include "s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_other_error_code code = 0;
  if (dst != NULL) {
    make_zero_decimal(dst);
    if (!isnan(src) && !isinf(src) && !-isinf(src)) {
      if (src != 0) {
        char str_num[100] = {0};
        sprintf(str_num, "%e", src);
        code = parser_string(str_num, dst);
        if (code) make_zero_decimal(dst);
      }
    } else
      code = other_ERROR;
  } else
    code = other_ERROR;
  return code;
}

int parser_string(char *str_num, s21_decimal *dst) {
  int error = 0, sign_dec = 0, degree = 0, degree_sign = 0, str_index = 0,
      int_index = 0;
  char degree_E[100] = {0}, str_int_num[100] = {0};
  if (str_num[0] == '-') {
    sign_dec = 1;
    str_index++;
  }

  for (; str_num[str_index] != '\0'; str_index++) {
    if (str_num[str_index] == 'e') {
      if (str_num[++str_index] == '-') degree_sign = 1;
      str_index++;
      for (int i = 0; str_num[str_index] != '\0'; str_index++) {
        degree_E[i] = str_num[str_index];
        degree_E[++i] = '\0';
      }
    } else if (str_num[str_index] != '.') {
      str_int_num[int_index] = str_num[str_index];
      str_int_num[++int_index] = '\0';
    }
  }
  int degree_E_int = atoi(degree_E);

  if (degree_E_int < 23) {
    if (degree_sign)
      degree = 6 + degree_E_int;
    else if (!degree_sign)
      degree = 6 - degree_E_int;

    unsigned int int_num = atoi(str_int_num);
    if (degree > 0) trailing_zeroes(&int_num, &degree);

    if (degree < 29) {
      s21_decimal dec_ten = {{10, 0, 0, 0}};
      dst->bits[0] = int_num;

      if (!degree_sign) {
        for (; degree < 0 && !error; degree++) {
          error = s21_mul(*dst, dec_ten, dst);
        }
        if (error) error = 1;
      }

      set_degree(dst, degree);
      set_sign(dst, sign_dec);
    } else
      error = 1;
  } else
    error = 1;
  return error;
}

void trailing_zeroes(unsigned int *str_num, int *degree) {
  int flag = 1;
  while (flag) {
    if (*str_num % 10 == 0) {
      *str_num /= 10;
      *degree -= 1;
    } else
      flag = 0;
  }
}