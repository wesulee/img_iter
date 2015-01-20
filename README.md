# img_iter

An attempt at [Roger Alsing's idea](http://rogeralsing.com/2008/12/07/genetic-programming-evolution-of-mona-lisa/).
For a working example of his idea, see [http://alteredqualia.com/visualization/evolve/](http://alteredqualia.com/visualization/evolve/).

### Dependencies
C++11 compiler, SDL2.


### Building

    make install

### Builds
[Windows](https://bitbucket.org/Wesulee/img_iter_build/get/master.zip)

### Todo
* Rewrite in Qt
* Support additional image formats

### Issues
* Only supports the (uncommented) Portable Pixel Map (ppm) image format
* Drawing is done without a library, so the image may look slightly different from other implementations
