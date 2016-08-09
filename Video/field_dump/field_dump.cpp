
#include <cstdio>
#include <cstdlib>

int  diff(unsigned char *a,  unsigned char *b, int nb)
{
    int  diff = 0;

    for(int i = 0; i < nb; i++)
    {
        diff += (abs(a[i]-b[i]));
    }
    return diff;
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Usage: ./field_dump width height <inp.yuv->4:2:0 only> <out.yuv>\n");
        exit(1);
    }

    FILE  *in;
    FILE *out;
    int   width  = atoi(argv[1]);
    int   height = atoi(argv[2]);
    unsigned char *field1;
    unsigned char *field2;
    int            frame_num = 0;

    field1 = new unsigned char[width*height*2];
    field2 = new unsigned char[width*height*2];

    in = fopen(argv[3], "r");
    if(in == NULL)
    {
        printf("Could not open file %s\n", argv[1]);
        exit(1);
    }

    out = fopen(argv[4], "w");
    if(out == NULL)
    {
        printf("Could not open file to write %s\n", argv[2]);
        exit(1);
    }

    while(!feof(in))
    {
        fseek(in, frame_num*width*height*1.5, SEEK_SET);

        for(int i = 0; i < height; i+=2)
        {
            fread(field1 + ((i/2)*width), 1, width, in);
            fread(field2 + ((i/2)*width), 1, width, in);
        }
        int d = diff(field1, field2, (width*(height/2)));
        printf("fnum=%d diff=%d\n", frame_num, d);
        unsigned char *field1_u = field1 + (width*(height/2));
        unsigned char *field2_u = field2 + (width * (height/2));

        for(int i = 0; i < height/2; i+=2)
        {
            fread(field1_u + ((i/2)*(width/2)), 1, width/2, in);
            fread(field2_u + ((i/2)*(width/2)), 1, width/2, in);
        }

        unsigned char *field1_v = field1_u + (width*(height/2))/4;
        unsigned char *field2_v = field2_u + (width * (height/2))/4;

        for(int i = 0; i < (height/2); i+=2)
        {
            fread(field1_v + ((i/2)*(width/2)), 1, width/2, in);
            fread(field2_v + ((i/2)*(width/2)), 1, width/2, in);
        }

        fwrite(field1, 1, width*(height/2)*1.5, out);
        fwrite(field2, 1, width*(height/2)*1.5, out);
        frame_num++;
    }

    fclose(in);
    fclose(out);

}
