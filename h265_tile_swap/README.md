
# Steps to create a replace a tile in h265 codec

## Requirements:

1. Get kvazaar from https://github.com/ultravideo/kvazaar.git and build binary

## Tile replacement example steps

Lets say we have a base h265 file which has
  - resolution -> 1920x1080
  - encoded as 4 tiles: 128x1792, 128x128
                        952x1792, 952x122
  
We are interested in replacing the top 128x128 tile with some content tile. The advantage we achieve is we can change a portion of the video without having to encode the entore 1920x1080 resolution.

The following steps illustrate creation of an example base h265 file and a replacement 128x128 tile. Also there is a script to replace a tile from the base file with the newly created tile.


1. Create a base h265 file using the following command

```
./src/kvazaar -i ~/sb_media/ChevyCruze_30s_ts.yuv --input-res 1920x1080 --tiles-width-split 1792 --tiles-height-split 128 --slice tiles --mv-constraint frametile -o c.h265
```

The above command creates 4 tiles. Lets say we want to modify the top right 128x128 of c.h265

2. An exammple ffmpeg crop command to crop the top right 128x128 potion og the video into a yuv file.

```
ffmpeg -i ChevyCruze_30s_ts.ts -vf "crop=128:128:1792:0" -pix_fmt yuv420p -f rawvideo  chevy_crop_128x128.yuv
```

3. Create a replacement 128x128 yuv file using the above command. Lets call this top_right.yuv. 

4. Create a new video with the resolution of c.h265 which is completely black except for the top right 128x128 which will have pixel values corresponding to chevy_crop_128x128.yuv. 
Let this video yuv sequence be black1920x1080_with_chevy_crop_128x128.yuv

5. The above black video can be created using the following command

```
./create_black_video_with_tile 128x128 chevy_crop_128x128.yuv 1792x0 1920x1080 black1920x1080_with_chevy_crop_128x128.yuv
```

create_black_video_with_tile is built using the following

```
g++ create_black_video_with_tile.cpp -o create_black_video_with_tile
```

6. Encode black1920x1080_with_chevy_crop_128x128.yuv with following command

```
./src/kvazaar -i black1920x1080_with_chevy_crop_128x128.yuv --input-res 1920x1080 --tiles-width-split 1792 --tiles-height-split 128 --slice tiles --no-wpp --mv-constraint frametile --input-fps 100 -o black1920x1080_with_chevy_crop_128x128.h265
```

7. To replace the corner 128x128 tile in c.h265 with the corner 128x128 tile in black1920x1080_with_chevy_crop_128x128.h265, run the following command

```
python h265_tile_swap.py -b c.h265 -r black1920x1080_with_chevy_crop_128x128.h265 -n 4 -bt 2 -rt 2 -o c_r.h265
```

8. The final output c_r.h265 is the intended output.

## References:
1. Useful h265 parser. https://gist.github.com/figgis/fd509a02d4b1aa89f6ef
