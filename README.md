## qdtide

A quick-and-dirty tide clock

Version 0.0.1, March 2017

# What is this?

qdtide is a very simple command-line utility for Linux
(and similar) for predicting high and low tides, when all the information
you have is some prior high tide time. 

It works on the same principle as a traditional mechanical tide clock -- 
a clock where the hour hand makes a full cycle in 12.42 hours. Naturally
it requires calibration, and the calibration will only be useful for
a specific location and a small region around it.

Unusually for any kind of predictive technology, a tide clock is generally more
accurate on average in the long term than the short.  In the short term, tides
are influenced by the position of the moon and the sun, and the weather.  In
the long term, the moon's effect predominates and, as the lunar month is pretty
constant, the tide behaviour tends to a regular cycle.

In the short term you have to accept that a tide clock, and this program,
could easily be wrong by an hour or more.
 
## Building and installing

The usual:

```
$ make
# make install
```

## Calibration

To use qdtide you need first to calibrate it by entering the time of a high
tide in the (not too distant) past. For example:

```
$ qdtide --base "7 4 2017 3:15"
```

The time format is "month day year hour:minute" so, in this case, 7th
April 2017, at 3:15 AM. The time must be in 24-hour format, although
the program uses 12-hour format for display by default.

This "base time" is stored in ``$HOME/.qdtide.rc``, and can be
used for any date in the future, for a specific location.

The base time can be based on actual observation, or taken from published tide
tables for the region -- if these are trustworthy.  For best results, the base
time should be taken at around the time of a full or new moon, because these
are the times when the solar and lunar effects on tide are in alignment. Of
course, if you're working by observation that might not be practicable. 

## Usage

With no arguments, qdtide displays the times of tides for the current day
(assuming that the base calibration time is in the past).  You can specify a
particular date if preferred:

```
$qdtide [day [month [year]]]
```

If either or both of the month or the year are omitted, these default to the 
current values. For example, 

```
$ qdtide 20 
```

shows tide predictions for the 20th day of the current month. 

Tides are assumed to be semi-diurnal, that is, there will be at most two high 
and two low tides in a 24-hour period, separated by about six hours.
If the ``--diurnal`` switch is used, there is taken to be
one high and low tide in a 24-hour period. Most parts of the world have
semi-diurnal tides, but there is no way to be sure without looking it up,
or living there. A few places have no tides at all, which makes prediction
infeasible -- and, of course, unnecessary. 

Times are displayed in 12-hour format with AM/PM, unless the 
``--tfh`` option is used.

## Legal, etc

qdtide is maintained by Kevin Boone, and distributed under the
terms of the GNU Public Licence, version 3.0. Essentially, you may do
whatever you like with it, provided the original author is acknowledged, and
you accept the risks involved in doing so. It goes without saying --
but I'm going to say it anyway -- that this program should not be used
for navigation calculations.

