entropy-gulp
===

Plot entropy for big files or whole disks, quickly.

Handles about 200MB per second on my laptop.

Existing tools I've found to graph entropy, like `binwalk`, are way too slow to use on whole disks. (I get about 3MB / second with `binwalk`.)

why?
---
I was overwriting a hard drive with random data before I recycle it.

(By the way, here's a trick to do that which is a whole lot faster than `/dev/urandom`. Urandom can only do about 10 MB / second, while this can do several hundred MB / second. Also shows a nice progress bar:
```
time openssl enc -aes-256-ctr -pass pass:"$(head -c 32 /dev/urandom | base64)" -nosalt < /dev/zero | pv | sudo dd of=/dev/sda1 bs=1M seek=160000
```

Then my computer died halfway through, so I wanted to measure entropy to see what the disk looked like. `binwalk` was sloooooow so I made this.

improvements
---
* Should use a less janky graphing library

* Could probably be even faster (limited by the read speed of my Samsung 830 SSD, about 400MB per second) if I made the reads asynchronous or put them onto a different thread 

credits
---
Using SDL-Grapher: https://code.google.com/archive/p/sdl-grapher/

License: GPLv3
