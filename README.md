vips-gmic
=========

libvips plugin for running gmic commands.

Build the plugin and install to `$VIPSHOME/lib/vips-plugins-8.9` (or whatever
version you are using) with:

  make debug
  make install

Then run with eg.:

  vips gmic ~/pics/k2.jpg x.jpg 20 1 1 -- "-verbose - -blur 10"

