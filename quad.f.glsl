uniform int width, height;
uniform float scale, x_shift, y_shift;

int x_resolution = 960, y_resolution = 2*x_resolution/3;
int maxiteration = 256;





void main(void) {
	//all of the meat of the mandelbrot set here
	//float c_re =  1 - 1.618033988749894848;
    //float c_im = 0;
    //float c_re = -.41;
    //float c_im = .59;
	float c_re = .285;
    float c_im = .02;
    int exponent = 2;

	float re = (3 * gl_FragCoord.x - 2*width + x_shift) / (x_resolution*scale);
    float im = (2 * gl_FragCoord.y - height + y_shift) / (y_resolution*scale);

    //re = x_shift;
    //im = y_shift;
	int k;
	for(k = 0; k < maxiteration; k++) {
		//_re is a placeholder for when re changes
		float _re = re;

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
	
	gl_FragColor = vec4(0.0, k == maxiteration ? 0.0 : float(k)/25.0, .3, 1.0);
}
