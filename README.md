# Lucidity

## Summary
This is a small hobby project to see if I really can build an operating system from scratch. I figure there will be lots of books to read and videos to watch along the way, so I'm not expecting to figure this out over night.

## Getting Started
I challenged myself to not require any dependencies outside of a few very ubiquitous ones (git, cc, Python, Make, and curl) that should already be built in and light weight for any user. Thus, to provide the dependencies (namely QEMU which Lucidity runs on) I had to build them from scratch...as well as *its* dependencies and so on. This was a long and arduous process but has thankfully resulted in a package that is entirely self contained and doesn't install **anything** on your system whatsoever.

Since this project uses git submodules, you have to first initialize and update those when you first clone the repo:
```console
$ git clone https://github.com/trenthuber/Lucidity.git
$ cd Lucidity
$ git submodule update --init --recursive # This will take a while
```

Actually building the project is far more straightforward. The following commands will build and run Lucidity:
```console
$ cc -o do cbs.c
$ ./do build
$ ./do lucidity run
```

The first time you build will take a while since it's building QEMU from scratch, but every subseqent time after, you can just run `./do lucidity run` and it will start up.
