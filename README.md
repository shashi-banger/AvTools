# AvTools
The repository will have all audio video tools. 

##Audio
- Silence detector: This command line tool allows user to detect long durations
  of silence in the input audio.  The output will be time offset in milliseconds 
  where the silence audio is present. An example output is as follows
  - channel=1, start=2603ms, duration=6869
  where channel is the audio channel where silence is observed, start is the
  start offset in milliseconds from where silence begins, duration is the
  duration in milliseconds of silence in the corresponding audio channel.
