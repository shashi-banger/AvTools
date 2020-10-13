
# Creating virtual camera and mic for Google Meet

## Sending Ffmpeg video to /dev/video0
Reference: https://askubuntu.com/questions/881305/is-there-any-way-ffmpeg-send-video-to-dev-video0-on-ubuntu
```
sudo apt install v4l2loopback-dkms
sudo modprobe v4l2loopback
v4l2-ctl --list-devices # This should list "Dummy video device"
```

## Creating virtual mic
Reference: https://stackoverflow.com/questions/61990828/how-to-redirect-an-audio-stream-to-a-virtual-pulseaudio-microphone-with-ffmpeg
```
pactl load-module module-null-sink sink_name="virtual_speaker" sink_properties=device.description="virtual_speaker"
pactl load-module module-remap-source master="virtual_speaker.monitor" source_name="virtual_mic" source_properties=device.description="virtual_mic"
```

## Ffmpeg command to decade a file and send the audio video data to virtual camera(v4l2) and virtual mic

Command to decode a stream real time and loop it infinitely.

```
PULSE_SINK=virtual_speaker ffmpeg -stream_loop -1 -re -i test_OTRK_201_BCS.ts -map 0:0 -vcodec rawvideo -pix_fmt yuv420p -f v4l2 /dev/video0 -map 0:1 -f pulse "out-sb"
```


## Google meet

Choose the camera option as "Dummy video device" and microphone option as "virtual_mic" in audio video settings. This will stream the ffmpeg output above to the meeting.
