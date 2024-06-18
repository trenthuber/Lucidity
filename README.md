# Lucidity

## Summary
This is a small hobby project to see if I really can build an operating system from scratch. I figure there will be lots of books to read and videos to watch along the way, so I'm not expecting to figure this out over night.

## Getting Started
I challenged myself to not require any dependencies outside of a few very ubiquitous ones (cc, git, Python, and Make) that should already be built in and light weight for any user. Thus, to provide the dependencies (namely QEMU which Lucidity runs on) I had to build them from scratch...as well as *its* dependencies and so on. This was a long and arduous process but has thankfully resulted in a package that is entirely self contained and doesn't install **anything** on your system whatsoever.

To build everything out of the box, simply run:
```console
$ cc -o lucidity cbs.c
$ ./lucidity build all run
```

The first time you build will take a while since it's building QEMU from scratch, but every subseqent time after, you can just run `./lucidity run` and it will start up.
