#include <stdio.h>
#include <stdlib.h>

int main() {
  double num1, num2;
  char operator;
  double result;

  printf("Simple Calculator\n");
  printf("-----------------\n");
  printf("Enter an expression (e.g., 4 + 5): ");

  // Read user input
  if (scanf("%lf %c %lf", &num1, &operator, & num2) != 3) {
    printf("Error: Invalid input format.\n");
    return 1;
  }

  // Perform the operation
  switch (operator) {
    case '+':
      result = num1 + num2;
      printf("Result: %.2lf\n", result);
      break;
    case '-':
      result = num1 - num2;
      printf("Result: %.2lf\n", result);
      break;
    case '*':
      result = num1 * num2;
      printf("Result: %.2lf\n", result);
      break;
    case '/':
      if (num2 == 0) {
        printf("Error: Division by zero is undefined.\n");
        return 1;
      }
      result = num1 / num2;
      printf("Result: %.2lf\n", result);
      break;
    default:
      printf("Error: Invalid operator '%c'. Please use +, -, *, or /.\n", operator);
      return 1;
  }

  return 0;
}
