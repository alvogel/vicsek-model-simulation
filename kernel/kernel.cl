typedef struct
{
    unsigned int n;
    float r;
    unsigned int width;
    unsigned int height;

} header;

typedef struct
{
    float x;
    float y;
    float vx;
    float vy;

} particle;

__kernel void vicsek(__global const header *H, __global const particle *A, __global particle *C)
{
    int index = get_global_id(0);

    float x = A[index].x;
    float y = A[index].y;

    float2 sum_v;
    sum_v.x = 0.0f;
    sum_v.y = 0.0f;

    float square_r = native_powr(H->r,2);

    for(int i=0; i<H->n; i++)
    {
        float dx_c = x - A[i].x;
        float dx_cs = dx_c * dx_c;

        float dy_c = y - A[i].y;
        float dy_cs = dy_c * dy_c;
        float distance_center = dx_cs + dy_cs;

        float dx_e = dx_c + H->width;
        float distance_east   = (dx_e * dx_e) + dy_cs;

        float dy_n = dy_c - H->height;
        float distance_north  = dx_cs + (dy_n * dy_n);

        float dx_w = dx_c - H->width;
        float distance_west   = (dx_w * dx_w) + dy_cs;

        float dy_s = dy_c + H->height;
        float distance_south  = dx_cs + (dy_s * dy_s);

        if(     distance_center < square_r ||
                distance_east < square_r ||
                distance_north < square_r ||
                distance_west < square_r ||
                distance_south < square_r
          )

        {
            sum_v.x += A[i].vx;
            sum_v.y += A[i].vy;
        }

    }

    C[index].vx = sum_v.x;
    C[index].vy = sum_v.y;

}
