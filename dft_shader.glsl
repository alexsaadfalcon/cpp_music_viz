#version 410

out vec4 colorOut;
uniform double screen_ratio;
uniform double C_re, C_im;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double zoom;
uniform int itr;
uniform int num_coeff;
uniform float coeff_float_arr[1000];
uniform float coeff_float_max;
bool julia = true;
int exponent = 2;

double absv(double f) {
    if (f < 0.0) {
        return f * -1.0;
    }
    return f;
}

vec4 map_to_color(double t) {
    double r = 9.0 * (1.0 - t) * t * t * t;
    double g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    double b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;

    return vec4(r, g, b, 1.0);
}

vec4 map_to_color2(double x, double max_x) {
    x = absv(x);
    double darkness = 0.5;
    return vec4((1-x/max_x)/darkness, 0.0, (x/max_x)/darkness, 1.0);
}

void main() {
    double re, im, c_re, c_im;
    double x  = screen_ratio * 1.0 * (gl_FragCoord.x / screen_size.x - 0.5);
    double y = (gl_FragCoord.y * 1.0 / screen_size.y - 0.5);
    x /= zoom;
    y /= zoom;
    x += center.x;
    y += center.y;
    double max_x = 8;
    if (x >= -max_x && x <= max_x && absv(y) < (coeff_float_arr[uint(absv(x) * double(num_coeff) / max_x / 2.0)]) / 1000000) {
        colorOut = map_to_color2(x, max_x);
    } else {
        colorOut = vec4(0.0, 0.0, 0.0, 1.0);
    }
}