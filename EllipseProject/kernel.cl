__kernel void draw_ellipse(__global uchar4* output, 
    const int width,
    const int height,
    const int x0, 
    const int y0,
    const int a,
    const int b){
    int x = get_global_id(0);
    int y = get_global_id(1);
    if (((x < x0 - 2 * a) || (x > x0 + 2 * a)) && ((y < y0 - 2 * b) || (y > y0 + 2 * b)))
        output[y * width + x] = (uchar4)(255, 255, 255, 255); 
    if ((((double)(x - x0) * (x - x0)) / (a * a) + ((double)(y - y0) * (y - y0)) / (b * b)) <= 1.0f)
        output[y * width + x] = (uchar4)(0, 155, 100, 100);
    else
        output[y * width + x] = (uchar4)(255, 255, 255, 255);
}