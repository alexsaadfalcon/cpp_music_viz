#version 410

out vec4 colorOut;
uniform double screen_ratio;
uniform double C_re, C_im;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double zoom;
uniform int itr;
bool julia = true;
int exponent = 2;

vec4 map_to_color(double t) {
    double r = 9.0 * (1.0 - t) * t * t * t;
    double g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    double b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;

    return vec4(r, g, b, 1.0);
}

void main()
{
    double re, im, c_re, c_im;
    if (!julia) {
        re = 0;
        im = 0;
        c_re = screen_ratio * 1.0 * (gl_FragCoord.x / screen_size.x - 0.5);
        c_im = (gl_FragCoord.y * 1.0 / screen_size.y - 0.5);

        c_re /= zoom;
        c_im /= zoom;

        c_re += center.x;
        c_im += center.y;
    } else {
        //c_re = -.41;
        //c_im = .59;
        //c_re = .295;
        //c_im = .02;
        //c_re =  1 - 1.618033988749894848;
        //c_im = 0;
        c_re = C_re;
        c_im = C_im;
        re = screen_ratio * 1.0 * (gl_FragCoord.x / screen_size.x - 0.5);
        im = (gl_FragCoord.y * 1.0 / screen_size.y - 0.5);

        re /= zoom;
        im /= zoom;

        re += center.x;
        im += center.y;
    }


    int k;
    for(k = 0; k < itr; k++) {
        //_re is a placeholder for when re changes
        double _re = re;

        //gl_FragCoord.x returns between 0 & width, etc.
        if (exponent == 2) {
            re = re*re - im*im;
            im = 2*_re*im;
        } else if (exponent == 3) {
            re = re*re*re - 3*im*im*re;
            im = -im*im*im + 3*im*_re*_re;
        } else if (exponent == 4) {
              re = re*re*re*re - 6*im*im*re*re + im*im*im*im;
              im = -4*_re*im*im*im + 4*im*_re*_re*_re;
        } else if (exponent == 5) {
            re = re*re*re*re*re - 10*re*re*re*im*im + 5*re*im*im*im*im;
            im = 5*_re*_re*_re*_re*im - 10*_re*_re*im*im*im + im*im*im*im*im;
        } else if (exponent == 6) {
            re = re*re*re*re*re*re - 15*re*re*re*re*im*im + 15*im*im*im*im*re*re - im*im*im*im*im*im;
            im = 6*_re*_re*_re*_re*_re*im - 20*_re*_re*_re*im*im*im + 6*_re*im*im*im*im*im;
        } else if (exponent == 7) {
            re = re*re*re*re*re*re*re - 21*re*re*re*re*re*im*im + 35*re*re*re*im*im*im*im - 7*re*im*im*im*im*im*im;
            im = 7*_re*_re*_re*_re*_re*_re*im - 35*_re*_re*_re*_re*im*im*im + 21*_re*_re*im*im*im*im*im - im*im*im*im*im*im*im;
        } else if (exponent == 8) {
            re = re*re*re*re*re*re*re*re - 28*re*re*re*re*re*re*im*im + 70*re*re*re*re*im*im*im*im - 28*re*re*im*im*im*im*im*im + im*im*im*im*im*im*im*im;
            im = 8*_re*_re*_re*_re*_re*_re*_re*im - 56*_re*_re*_re*_re*_re*im*im*im + 56*_re*_re*_re*im*im*im*im*im - 8*_re*im*im*im*im*im*im*im;
        }

        re = re + c_re;
        im = im + c_im;
        //take magnitude of re and im
        if(re*re + im*im > 4) break;
    }

    //colorOut = vec4(1-k/double(itr), 0.0, k/double(itr), 1.0);
    //colorOut = vec4(k/double(itr), 0.4, 1-k/double(itr), 1.0);
    colorOut = map_to_color(k == itr ? 1 : k/double(50));
}