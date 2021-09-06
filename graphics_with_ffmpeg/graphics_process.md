# Example graphics processing flow

## Test videos

Big Buck Bunny: https://test-videos.co.uk/vids/bigbuckbunny/mp4/h264/1080/Big_Buck_Bunny_1080_10s_30MB.mp4

## Graphics Video Generation

Examples of commands to take a sequence of png images and convert them into a
video of predefined duration.

1. Ffmpeg command to embed a single png graphics file into video.

```bash

ffmpeg -f lavfi  -i color=color=black:s=1920x1080:r=30  -loop 1 -i aston_1920x128.png   -filter_complex "[0][1]overlay=x=0:y=952[a]" -t 20 -map "[a]" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -keyint_min 15 graphics_video.mov
```

2. Ffmpegd command to embed a sequence of png graphics into video

- Generation of sequence

```bash
for i in `seq 001 029`;do dest=`printf "aston_1920x128_%03d.png\n" $i`;cp aston_1920x128_000.png $dest; done

for i in `seq 31 59`;do dest=`printf "aston_1920x128_%03d.png\n" $i`;cp aston_1920x128_030.png $dest; done
```

- Video conversion command

```bash
ffmpeg -f lavfi  -i color=color=black:s=1920x1080:r=30  -loop 1 -i aston_1920x128_%03d.png   -filter_complex "[0][1]overlay=x=0:y=952[a]" -t 20 -map "[a]" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -keyint_min 15 graphics_video.mov
```


## Graphics Video on Master Input

Crops an appropriate portion of graphics video and overlays the cropped video on the input
video at appropriate offset. This overlay can be enabled only between a pre-defined 
interval within the input video.

1. Example command 

```bash
ffmpeg -i bbb.ts -i graphics_video.mov -filter_complex "[1:v]crop=x=0:y=950:w=1920:h=128[c];[0:v][c]overlay=x=0:y=952:enable='between(t,2,7)'[a]" -map "[a]" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -t 10 -keyint_min 15 1.mov
```

