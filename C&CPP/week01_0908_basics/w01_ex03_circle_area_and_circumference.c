/*
Read a radius and print the area and circumference of a circle.
- Use a global constant PI (3.14).
- Implement area() and circumference() functions.
- Input and output are handled in main().
*/
#include <stdio.h>
#define PI 3.14
double area(double radius) {
    return PI * radius * radius;
}
double circumference(double radius) {
    return 2 * PI * radius;
}
int main() {
    double radius;
    printf("Enter the radius to compute area/circumference:");
    scanf("%lf", &radius);
    printf("For radius %.6f:\n", radius);
    printf("Area: %.6f\n", area(radius));
    printf("Circumference: %.6f\n", circumference(radius));
    return 0;
}