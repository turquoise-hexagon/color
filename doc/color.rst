-----
color
-----

edit colors
===========

:date: April 2020
:version: 0.0
:manual section: 1
:manual group: General Commands Manual

synopsis
--------
color [-rgba `<amount>`] `<string>`

description
-----------
colors edits colors passed to it as hexadecimal values

it allows for changing rgb values & overall brightness

options
-------
-r `<amount>` `<string>`
    change red value of all hex colors in <string> by <amount>
-g `<amount>` `<string>`
    change green value of all hex colors in <string> by <amount>
-b `<amount>` `<string>`
    change blue value of all hex colors in <string> by <amount>
-a `<amount>` `<string>`
    change overall value of all hex colors in <string> by <amount>

example
-------
::

    $ color -r 100 -g -100 '#021B21'
    #660021
    $ color -a 100 '#021B21'
    #667F85
    $ echo '#021B21' | color -r 100 -g 100 -b 100
    #667F85
