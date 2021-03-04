
# Creating virtual camera and mic for Google Meet

## Build custom kernel with V4L2 support

AWS default ubuntu AMI seems to have a kernel without support for "Video for Linux"(V4L2). 

Move to a generic linux
```
sudo apt-get install linux-generic
```

Fetch linux source(nearest version) code from "https://git.launchpad.net/~canonical-kernel/ubuntu/+source/linux-aws/+git/bionic/"

Follow steps [here](https://www.cyberciti.biz/tips/compiling-linux-kernel-26.html) to build a custom linux kernel with supoort
for "Video for Linux".

When "make menuconfig" is run as mentioned enable the following:
- "Driver Drivers.Multimedia Support.Cameras/video grabberssupport"
- "Driver Drivers.Multimedia Support.V4L2 sub-device userspace API"
- "Driver Drivers.Multimedia Support.V4L platform devices"


## Sending Ffmpeg video to /dev/video0
Reference: https://askubuntu.com/questions/881305/is-there-any-way-ffmpeg-send-video-to-dev-video0-on-ubuntu
```
sudo apt install v4l2loopback-dkms
sudo modprobe v4l2loopback exclusive_caps=1
v4l2-ctl --list-devices # This should list "Dummy video device"
```

Steps to build v4l2loopback driver for custome kernel are as below:
```
sudo apt-get install build-essential checkinstall
wget https://github.com/umlaeute/v4l2loopback/archive/master.zip
unzip master.zip
cd v4l2loopback-master
make
sudo make install
sudo depmod -a
sudo modprobe v4l2loopback
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
