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
color `[option]` <parameter>

description
-----------
colors edits colors passed to it as hexadecimal values

it allows for changing rgb values & overall brightness

options
-------
``-r <amount> <hex>``
    modify red value of <hex> by <amount>
``-g <amount> <hex>``
    modify green value of <hex> by <amount>
``-b <amount> <hex>``
    modify blue value of <hex> by <amount>
``-a <amount> <hex>``
    modify all values of <hex> by <amount>

example
-------
::

    $ color -r 100 -g -100 '#021B21'
    #660021
    $ color -a 100 '#021B21'
    #667F85
    $ echo '#021B21' | color -r 100 -g 100 -b 100
    #667F85
