#include "inc/math_functions.h"

double sqrt(double x) {
    double result = 1;
    double term = 1;

    for (int i = 0; i < 10; i++) {
        term = (term + x / term) / 2;
        result = term;
    }

    return result;
}

double fmod(double x, double y) {
    return x - y * (int)(x / y);
}

double cos(double angle) {
    double result = 1;
    double term = 1;
    double angleSquared = angle * angle;
    int sign = -1;

    for (int i = 2; i <= 20; i += 2) {
        term *= angleSquared / (i * (i - 1));
        result += sign * term;
        sign = -sign;
    }

    return result;
}

double sin(double angle) {
    double result = angle;
    double term = angle;
    double angleSquared = angle * angle;
    int sign = -1;

    for (int i = 3; i <= 21; i += 2) {
        term *= angleSquared / (i * (i - 1));
        result += sign * term;
        sign = -sign;
    }

    return result;
}

double tan(double angle) {
    return sin(angle) / cos(angle);
}

double fabs(double x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

double acos(double x) {
    if (x < -1 || x > 1) {
        return 0; // Error case
    }
    double result = 0;
    double term = x;
    double angle = 0;

    for (int i = 1; i <= 20; i++) {
        term *= (x * x) / ((2 * i - 1) * (2 * i));
        angle += term / (2 * i + 1);
    }

    result = M_PI / 2 - angle;
    return result;
}

double asin(double x) {
    if (x < -1 || x > 1) {
        return 0;
    }
    
    if (x > 0.8 || x < -0.8) {
        return M_PI / 2 - acos(x);
    }
    
    double result = x;
    double term = x;
    double x_squared = x * x;
    
    for (int i = 1; i <= 10; i++) {
        double coef = (2 * i - 1) / (2.0 * i);
        term *= x_squared * coef;
        result += term / (2 * i + 1);
    }
    
    return result;
}

double atan(double x) {
    double result = 0;
    int sign = 1;
    
    if (x > 1) {
        return M_PI / 2 - atan(1 / x);
    }
    if (x < -1) {
        return -M_PI / 2 - atan(1 / x);
    }
    
    double term = x;
    double x_squared = x * x;
    
    for (int i = 1; i <= 20; i += 2) {
        result += sign * term / i;
        term *= x_squared;
        sign = -sign;
    }
    
    return result;
}

double atan2(double y, double x) {
    if (x > 0) {
        return atan(y / x);
    } else if (x < 0) {
        if (y >= 0) {
            return atan(y / x) + M_PI;
        } else {
            return atan(y / x) - M_PI;
        }
    } else { 
        if (y > 0) {
            return M_PI / 2;
        } else if (y < 0) {
            return -M_PI / 2;
        } else { 
            return 0; 
        }
    }
}

int fast_exp(int x, int y) { // returns x^y
    if (y == 0) return 1;
    if (y == 1) return x;
    if (y % 2 == 0) {
        int p = fast_exp(x, y / 2);
        return p * p;
    }
    else {
        int p = fast_exp(x, y / 2);
        return x * p * p;
    }


}