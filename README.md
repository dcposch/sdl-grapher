entropy-gulp
===

Plot entropy for big files or whole disks, quickly.

Handles about 200MB per second on my laptop.

Existing tools I've found to graph entropy, like `binwalk`, are way too slow to use on whole disks. (I get about 3MB / second with `binwalk`.)

example
---
![](http://i.imgur.com/vpeGdHz.png)

```
$ ./entropy-gulp /tmp/test-mixed-1G
Processed 100MB, min entropy so far: 0.000000
Processed 200MB, min entropy so far: 0.000000
Processed 300MB, min entropy so far: 0.000000
Processed 400MB, min entropy so far: 0.000000
Processed 500MB, min entropy so far: 0.000000
Processed 600MB, min entropy so far: 0.000000
Processed 700MB, min entropy so far: 0.000000
Processed 800MB, min entropy so far: 0.000000
Processed 900MB, min entropy so far: 0.000000
Done. Processed 991MB, min entropy: 0.000000
```

Here, `test-mixed-1G` is a 1GB file of alternating ~20MB random bytes and zero bytes.

example #2
---
```
$ sudo ./entropy-gulp /dev/sda1
Processed 100MB, min entropy so far: 0.999984
[after a few minutes...]
Done. Processed 524288MB, min entropy: 0.999968
```

why?
---
I was overwriting a hard drive with random data before I recycle it.

(By the way, here's a trick to do that which is a whole lot faster than `/dev/urandom`. Urandom can only do about 10 MB / second, while this can do several hundred MB / second. Also shows a nice progress bar:
```
time openssl enc -aes-256-ctr -pass pass:"$(head -c 32 /dev/urandom | base64)" -nosalt < /dev/zero | pv | sudo dd of=/dev/sda[Drive to erase] bs=1M
```

Then my computer froze halfway through, so I wanted to measure entropy to see what the disk looked like. `binwalk` was sloooooow so I made this.

improvements
---
* Should use a less janky graphing library

* On a related note... axes should be labeled

* Should be statically linked, so that we can provide a binary each for Mac, Windows and Linux that people can just paste and use.

* Should handle small files. Right now it doesn't support files <1GB cleanly.

* Could probably be even faster (limited by the read speed of my Samsung 830 SSD, about 400MB per second) if I made the reads asynchronous or put them onto a different thread 

credits
---
Using SDL-Grapher: https://code.google.com/archive/p/sdl-grapher/

License: GPLv3
