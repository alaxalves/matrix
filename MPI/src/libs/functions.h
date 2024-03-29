// Including base libs
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
// Including additional libs
#include <assert.h>
#include <math.h>
#include <string.h>
// Defining needed vars
typedef struct {
    float r;
    float i;
} complex;

static complex ctmp;
#define C_SWAP(a,b) {ctmp=(a);(a)=(b);(b)=ctmp;}
#define N 512

// Fast Fourier Transform (FFT) 1 dimension

/*
 ------------------------------------------------------------------------
 FFT1D            c_fft1d(r,i,-1)
 Inverse FFT1D    c_fft1d(r,i,+1)
 ------------------------------------------------------------------------
*/
/* ---------- FFT 1D
   This computes an in-place complex-to-complex FFT
   r is the real and imaginary arrays of n=2^m points.
   isign = -1 gives forward transform
   isign =  1 gives inverse transform
*/

void c_fft1d(complex *r, int n, int isign) {
    int m=0, i=0, i1=0, j=0, k=0, i2=0, l=0, l1=0, l2=0;
    float c1, c2, z;
    complex t, u;

    if (isign == 0) {
        return;
    }

    /* Do the bit reversal */
    i2 = n >> 1;
    j = 0;
    for (i=0;i<n-1;i++) {
        if (i < j)
        C_SWAP(r[i], r[j]);
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* m = (int) log2((double)n); */
    for (i=n,m=0; i>1; m++,i/=2);

    /* Compute the FFT */
    c1 = -1.0;
    c2 =  0.0;
    l2 =  1;
    for (l=0;l<m;l++) {
        l1   = l2;
        l2 <<= 1;
        u.r = 1.0;
        u.i = 0.0;
        for (j=0; j<l1; j++) {
            for (i=j; i<n; i+=l2) {
                i1 = i + l1;

                /* t = u * r[i1] */
                t.r = u.r * r[i1].r - u.i * r[i1].i;
                t.i = u.r * r[i1].i + u.i * r[i1].r;

                /* r[i1] = r[i] - t */
                r[i1].r = r[i].r - t.r;
                r[i1].i = r[i].i - t.i;

                /* r[i] = r[i] + t */
                r[i].r += t.r;
                r[i].i += t.i;
            }
            z =  u.r * c1 - u.i * c2;

            u.i = u.r * c2 + u.i * c1;
            u.r = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (isign == -1) /* FWD FFT */
        c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Scaling for inverse transform */
    if (isign == 1) {       /* IFFT*/
        for (i=0;i<n;i++) {
            r[i].r /= n;
            r[i].i /= n;
        }
    }
}

void getData(char fileName[15], complex **data){
    FILE *file = fopen(fileName, "r");

    int i=0, j=0, result=0;

    for (i=0; i<N; i++){
        for (j=0; j<N; j++){
            result = fscanf(file,"%g",&data[i][j].r);
            data[i][j].i = 0.00;
        }
    }

    fclose(file);
}

void transpose(complex **data, complex ** transp) {
    int i=0, j=0;
    for (i=0; i<N; i++)
      for(j=0; j<N; j++)
         transp[j][i] = data[i][j];
}

void mmpoint(complex **data1, complex **data2, complex **data3) {

    int i=0, j=0;

    float real, imag;

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            data3[i][j].r = (data1[i][j].r * data2[i][j].r) - (data1[i][j].i * data2[i][j].i);
            data3[i][j].i = (data1[i][j].r * data2[i][j].i) + (data1[i][j].i * data2[i][j].r);
        }
    }
}

void printfile(char fileName[15], complex **data){

    FILE *file = fopen(fileName, "w");

    int i=0, j=0;

    for (i=0; i<N; i++) {
        for (j=0; j<N; j++){
            fprintf(file,"   %.7e",data[i][j].r);
        }
        fprintf(file,"\n");
    }

    fclose(file);
}