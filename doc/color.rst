-----
color
-----

edit colors
===========

:date: February 2021
:version: 0.0
:manual section: 1
:manual group: General Commands Manual

synopsis
--------
`<input>` | color [-rgbcmykhsl `<value>`]

description
-----------
color edits all colors passed to it as hex codes

it allows for editing rgb, cmyk and hsl values

example
-------
::

    $ echo '#021b21' | color -r 100 -g -100
    #660021
    $ echo '#C9647E' | color -l 0.1 -s -0.1
    #ce92a1
