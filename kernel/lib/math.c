float isqrt(float num) {
    long i;
    float x, y;

    x = num * 0.5F;
    y  = num;
    i  = *(long*)&y;
    i  = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y  = y * (1.5F - (x * y * y));
    y  = y * (1.5F - (x * y * y));

    return y;
}

float sqrt(float num) {
    return 1.0 / isqrt(num);
}
