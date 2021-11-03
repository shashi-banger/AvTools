# Example graphics processing flow

## Test videos

Big Buck Bunny: https://test-videos.co.uk/vids/bigbuckbunny/mp4/h264/1080/Big_Buck_Bunny_1080_10s_30MB.mp4

## Graphics Video Generation

Examples of commands to take a sequence of png images and convert them into a
video of predefined duration.

1. Ffmpeg command to embeds a single png graphics file into black video whose duration can be
controlled usin *-t* option of ffmpeg.

```bash

ffmpeg -f lavfi  -i color=color=black:s=1920x1080:r=30  -loop 1 -i aston_1920x128.png   -filter_complex "[0][1]overlay=x=0:y=952[a]" -t 20 -map "[a]" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -keyint_min 15 graphics_video.mov
```

2. Ffmpeg command to embed a sequence of png graphics into video

- Generation of sequence from single png

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

2. Example where a graphics video of 1280x720 should be overlaid on original video which is 1920x1080 to result in 120x720 resolution output video. 
The original video is being scaled down to 1280x720 and then the the lower third graphics is overlaid on input video in the following command.

```bash
# First scale the above graphics video to 1920x1080
ffmpeg -i graphics_video.mov -vf "scale=w=1280:h=720" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -keyint_min 15 graphics_video_1280x720.mov

# Overlay graphics on bbb.ts which 1920x1080 resolution. Note the change in w,h,x,y parameters
ffmpeg -i bbb.ts -i graphics_video_1280x720.mov -filter_complex "[0:v]scale=w=1280:h=720[s];[1:v]crop=x=0:y=608:w=1280:h=80[c];[s][c]overlay=x=0:y=608:enable='between(t,2,7)'[a]" -map "[a]" -c:v h264 -profile:v high -b:v 2000000 -bf 0 -g 15 -t 10 -keyint_min 15 1.mov
```

## Kvazaar Command to encode Video

```bash
ffmpeg -i ~/sb_media/Big_Buck_Bunny_1080_10s_30MB.mp4 -vcodec libkvazaar -kvazaar-params tiles-width-split="128,1792",tiles-height-split="832,960",slice=tiles,mv-constraint=frametile -vb 3000000 ~/sb_media/bbb.mov
```

