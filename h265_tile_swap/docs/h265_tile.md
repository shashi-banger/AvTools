
1. Created V1.h265: Encoded a 1920x1080 using h265 encoder https://github.com/ultravideo/kvazaar. This video has 2x2 tiles. Encoding command:
    kvazaar -i /tmp/testsrc.yuv --input-res 1920x1080 --tiles 2x2 --mv-constraint frametile --slice tiles -o /tmp/V1.h265

2. Created V2.h265: 19020x1080 Colorband coded exactly same way as in 1

3. Replace every 4th VCL slice in V1.h265 with the 4th VCL slice in V2.h265. This is to be done. 

4. Result: V1.h265 with 4th quadrant being a color band.
