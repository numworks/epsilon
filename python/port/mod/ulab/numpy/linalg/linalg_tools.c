/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2010 Zoltán Vörös
*/

#include <math.h>
#include <string.h>
#include "py/runtime.h"

#include "linalg_tools.h"

/* 
 * The following function inverts a matrix, whose entries are given in the input array 
 * The function has no dependencies beyond micropython itself (for the definition of mp_float_t),
 * and can be used independent of ulab.
 */

bool linalg_invert_matrix(mp_float_t *data, size_t N) {
    // returns true, of the inversion was successful,
    // false, if the matrix is singular

    // initially, this is the unit matrix: the contents of this matrix is what
    // will be returned after all the transformations
    mp_float_t *unit = m_new(mp_float_t, N*N);
    mp_float_t elem = 1.0;
    // initialise the unit matrix
    memset(unit, 0, sizeof(mp_float_t)*N*N);
    for(size_t m=0; m < N; m++) {
        memcpy(&unit[m * (N+1)], &elem, sizeof(mp_float_t));
    }
    for(size_t m=0; m < N; m++){
        // this could be faster with ((c < epsilon) && (c > -epsilon))
        if(MICROPY_FLOAT_C_FUN(fabs)(data[m * (N+1)]) < LINALG_EPSILON) {
            //look for a line to swap
            size_t m1 = m + 1;
            for(; m1 < N; m1++) {
                if(!(MICROPY_FLOAT_C_FUN(fabs)(data[m1*N + m]) < LINALG_EPSILON)) {
                    for(size_t m2=0; m2 < N; m2++) {
                        mp_float_t swapVal = data[m*N+m2];
                        data[m*N+m2] = data[m1*N+m2];
                        data[m1*N+m2] = swapVal;
                        swapVal = unit[m*N+m2];
                        unit[m*N+m2] = unit[m1*N+m2];
                        unit[m1*N+m2] = swapVal;
                    }
                    break;
                }
            }
            if (m1 >= N) {
                m_del(mp_float_t, unit, N*N);
                return false;
            }
        }
        for(size_t n=0; n < N; n++) {
            if(m != n){
                elem = data[N * n + m] / data[m * (N+1)];
                for(size_t k=0; k < N; k++) {
                    data[N * n + k] -= elem * data[N * m + k];
                    unit[N * n + k] -= elem * unit[N * m + k];
                }
            }
        }
    }
    for(size_t m=0; m < N; m++) {
        elem = data[m * (N+1)];
        for(size_t n=0; n < N; n++) {
            data[N * m + n] /= elem;
            unit[N * m + n] /= elem;
        }
    }
    memcpy(data, unit, sizeof(mp_float_t)*N*N);
    m_del(mp_float_t, unit, N * N);
    return true;
}

/* 
 * The following function calculates the eigenvalues and eigenvectors of a symmetric 
 * real matrix, whose entries are given in the input array. 
 * The function has no dependencies beyond micropython itself (for the definition of mp_float_t),
 * and can be used independent of ulab.
 */

size_t linalg_jacobi_rotations(mp_float_t *array, mp_float_t *eigvectors, size_t S) {
    // eigvectors should be a 0-array; start out with the unit matrix
    for(size_t m=0; m < S; m++) {
        eigvectors[m * (S+1)] = 1.0;
    }
    mp_float_t largest, w, t, c, s, tau, aMk, aNk, vm, vn;
    size_t M, N;
    size_t iterations = JACOBI_MAX * S * S;
    do {
        iterations--;
        // find the pivot here
        M = 0;
        N = 0;
        largest = 0.0;
        for(size_t m=0; m < S-1; m++) { // -1: no need to inspect last row
            for(size_t n=m+1; n < S; n++) {
                w = MICROPY_FLOAT_C_FUN(fabs)(array[m * S + n]);
                if((largest < w) && (LINALG_EPSILON < w)) {
                    M = m;
                    N = n;
                    largest = w;
                }
            }
        }
        if(M + N == 0) { // all entries are smaller than epsilon, there is not much we can do...
            break;
        }
        // at this point, we have the pivot, and it is the entry (M, N)
        // now we have to find the rotation angle
        w = (array[N * S + N] - array[M * S + M]) / (MICROPY_FLOAT_CONST(2.0)*array[M * S + N]);
        // The following if/else chooses the smaller absolute value for the tangent
        // of the rotation angle. Going with the smaller should be numerically stabler.
        if(w > 0) {
            t = MICROPY_FLOAT_C_FUN(sqrt)(w*w + MICROPY_FLOAT_CONST(1.0)) - w;
        } else {
            t = MICROPY_FLOAT_CONST(-1.0)*(MICROPY_FLOAT_C_FUN(sqrt)(w*w + MICROPY_FLOAT_CONST(1.0)) + w);
        }
        s = t / MICROPY_FLOAT_C_FUN(sqrt)(t*t + MICROPY_FLOAT_CONST(1.0)); // the sine of the rotation angle
        c = MICROPY_FLOAT_CONST(1.0) / MICROPY_FLOAT_C_FUN(sqrt)(t*t + MICROPY_FLOAT_CONST(1.0)); // the cosine of the rotation angle
        tau = (MICROPY_FLOAT_CONST(1.0)-c)/s; // this is equal to the tangent of the half of the rotation angle

        // at this point, we have the rotation angles, so we can transform the matrix
        // first the two diagonal elements
        // a(M, M) = a(M, M) - t*a(M, N)
        array[M * S + M] = array[M * S + M] - t * array[M * S + N];
        // a(N, N) = a(N, N) + t*a(M, N)
        array[N * S + N] = array[N * S + N] + t * array[M * S + N];
        // after the rotation, the a(M, N), and a(N, M) entries should become zero
        array[M * S + N] = array[N * S + M] = MICROPY_FLOAT_CONST(0.0);
        // then all other elements in the column
        for(size_t k=0; k < S; k++) {
            if((k == M) || (k == N)) {
                continue;
            }
            aMk = array[M * S + k];
            aNk = array[N * S + k];
            // a(M, k) = a(M, k) - s*(a(N, k) + tau*a(M, k))
            array[M * S + k] -= s * (aNk + tau * aMk);
            // a(N, k) = a(N, k) + s*(a(M, k) - tau*a(N, k))
            array[N * S + k] += s * (aMk - tau * aNk);
            // a(k, M) = a(M, k)
            array[k * S + M] = array[M * S + k];
            // a(k, N) = a(N, k)
            array[k * S + N] = array[N * S + k];
        }
        // now we have to update the eigenvectors
        // the rotation matrix, R, multiplies from the right
        // R is the unit matrix, except for the
        // R(M,M) = R(N, N) = c
        // R(N, M) = s
        // (M, N) = -s
        // entries. This means that only the Mth, and Nth columns will change
        for(size_t m=0; m < S; m++) {
            vm = eigvectors[m * S + M];
            vn = eigvectors[m * S + N];
            // the new value of eigvectors(m, M)
            eigvectors[m * S + M] = c * vm - s * vn;
            // the new value of eigvectors(m, N)
            eigvectors[m * S + N] = s * vm + c * vn;
        }
    } while(iterations > 0);
    
    return iterations;
}
