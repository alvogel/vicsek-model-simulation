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

    float2 sum_v;
    sum_v.x = 0;
    sum_v.y = 0;

    float square_r = pown(H->r,2);

    for(int i=0; i<H->n; i++)
    {

        float distance_center;
        float distance_east;
        float distance_north;
        float distance_west;
        float distance_south;

        float dx_c,dy_c;
        float dx_cs,dy_cs;
        float dx_e;
        float dy_n;
        float dx_w;
        float dy_s;

        dx_c = A[index].x - A[i].x;
        dx_cs = dx_c * dx_c;
        dy_c = A[index].y - A[i].y;
        dy_cs = dy_c * dy_c;
        distance_center = dx_cs + dy_cs;

        dx_e = dx_c + H->width;
        distance_east   = (dx_e * dx_e) + dy_cs;

        dy_n = dy_c - H->height;
        distance_north  = dx_cs + (dy_n * dy_n);

        dx_w = dx_c - H->width;
        distance_west   = (dx_w * dx_w) + dy_cs;

        dy_s = dy_c + H->height;
        distance_south  = dx_cs + (dy_s * dy_s);

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

    float2 norm_v = fast_normalize(sum_v);

    C[index].vx = norm_v.x;
    C[index].vy = norm_v.y;

}
