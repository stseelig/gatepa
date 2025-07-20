# gatepa

ApeTag tool


## Tutorial

Let us pretend that we have just ripped a CD with cd-paranoia(1),
and now want to tag it.
```
$ ls
track01.cdda.tta  track04.cdda.tta  track07.cdda.tta  track10.cdda.tta
track02.cdda.tta  track05.cdda.tta  track08.cdda.tta
track03.cdda.tta  track06.cdda.tta  track09.cdda.tta

$ gatepa ./*.tta -- print/

01:0048EA9E:'./track01.cdda.tta'
	(tagless)

02:010E4538:'./track02.cdda.tta'
	(tagless)

03:0142DC15:'./track03.cdda.tta'
	(tagless)

04:027FF7F6:'./track04.cdda.tta'
	(tagless)

05:0183E518:'./track05.cdda.tta'
	(tagless)

06:02ABDD58:'./track06.cdda.tta'
	(tagless)

07:02C1B1EF:'./track07.cdda.tta'
	(tagless)

08:014BD73A:'./track08.cdda.tta'
	(tagless)

09:01635F5D:'./track09.cdda.tta'
	(tagless)

10:00C201D9:'./track10.cdda.tta'
	(tagless)

```
As we can see, the files have no tags.


A gatepa command is built like
```
$ gatepa [options] [files] -- [modes]
```
(The "--" is necessary.)


A gatepa mode is used like
```
mode$[file-range]$[field1]$[field2][$]
```
where '$' is the seperator byte used.
(Some bytes are unallowed:
'-', ',', or the PATH seperator when 'add-file'ing.)


```
$ gatepa ./*.tta -- \
	add//artist/'Pink Floyd' \
	add//album/'The Dark Side of the Moon' \
	print/

01:0048EA9E:'./track01.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

02:010E4538:'./track02.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

03:0142DC15:'./track03.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

04:027FF7F6:'./track04.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

05:0183E518:'./track05.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

06:02ABDD58:'./track06.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

07:02C1B1EF:'./track07.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

08:014BD73A:'./track08.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

09:01635F5D:'./track09.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

10:00C201D9:'./track10.cdda.tta'
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon

```


The 'add' mode is used to add/replace text items to/in tags.
```
add$[file-range]$key$value[$]
```


No file has been written to, yet. In order to write the tags, we have to use
the 'write' mode. We can use 'print' to see the current state of the tags
at any time.
```
print$[file-range][$]
```
Ranges can be written as 'start-finish', 'start-', '-finish', or 'index'
where 'start', 'finish', and 'index' are the 1-based index of a file.
A blank range means the full range (every file).
Multiple ranges may be specified via comma-seperation.


```
$ gatepa ./*.tta -- \
	add//artist/'Pink Floyd' \
	add//album/'The Dark Side of the Moon' \
	add//year/1973 \
	auto-track/ \
	add//genre/Rock \
	add//comment/'Capital Records - CDP 7243 5 82136 2 1' \
	add/1/title/'Speak to Me' \
	add/2/title/'Breathe' \
	add/3/title/'On the Run' \
	add/4/title/Time \
	add/5/title/'The Great Gig in the Sky' \
	add/6/title/Money \
	add/7/title/'Us and Them' \
	add/8/title/'Any Colour You Like' \
	add/9/title/'Brain Damage' \
	add/10/title/Eclipse \
	append/1,4/credit/Mason \
	append/2,4,6,7,9,10/credit/Waters \
	append/2,3,8/credit/Gilmour \
	append/8/credit/Mason \
	append/2,4,5,7,8/credit/Wright \
	append/3/credit/Waters \
	append/4/credit/Gilmour \
	sort/ \
	verify/ \
	write/ \
	print/

01:0048EA9E:'./track01.cdda.tta'
        title...................: Speak to Me
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 01/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit..................: Mason

02:010E4538:'./track02.cdda.tta'
        title...................: Breathe
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 02/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit...............[0]: Waters
                             [1]: Gilmour
                             [2]: Wright

03:0142DC15:'./track03.cdda.tta'
        title...................: On the Run
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 03/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit...............[0]: Gilmour
                             [1]: Waters

04:027FF7F6:'./track04.cdda.tta'
        title...................: Time
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 04/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit...............[0]: Mason
                             [1]: Waters
                             [2]: Wright
                             [3]: Gilmour

05:0183E518:'./track05.cdda.tta'
        title...................: The Great Gig in the Sky
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 05/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit..................: Wright

06:02ABDD58:'./track06.cdda.tta'
        title...................: Money
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 06/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit..................: Waters

07:02C1B1EF:'./track07.cdda.tta'
        title...................: Us and Them
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 07/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit...............[0]: Waters
                             [1]: Wright

08:014BD73A:'./track08.cdda.tta'
        title...................: Any Colour You Like
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 08/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit...............[0]: Gilmour
                             [1]: Mason
                             [2]: Wright

09:01635F5D:'./track09.cdda.tta'
        title...................: Brain Damage
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 09/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit..................: Waters

10:00C201D9:'./track10.cdda.tta'
        title...................: Eclipse
        artist..................: Pink Floyd
        album...................: The Dark Side of the Moon
        year....................: 1973
        track...................: 10/10
        genre...................: Rock
        comment.................: Capital Records - CDP 7243 5 82136 2 1
        credit..................: Waters

```
(I 'append'ed 'credit' more than necessary,
because I wanted the booklet order.)


We can confirm that the tags are written with 'dump'.
Unlike 'print', 'dump' will output the raw tag as it currently is on disk.
```
$ gatepa ./*.tta -- dump/4 | hexdump -C
00000000  41 50 45 54 41 47 45 58  d0 07 00 00 08 01 00 00  |APETAGEX........|
00000010  08 00 00 00 00 00 00 e0  00 00 00 00 00 00 00 00  |................|
00000020  04 00 00 00 00 00 00 00  74 69 74 6c 65 00 54 69  |........title.Ti|
00000030  6d 65 0a 00 00 00 00 00  00 00 61 72 74 69 73 74  |me........artist|
00000040  00 50 69 6e 6b 20 46 6c  6f 79 64 19 00 00 00 00  |.Pink Floyd.....|
00000050  00 00 00 61 6c 62 75 6d  00 54 68 65 20 44 61 72  |...album.The Dar|
00000060  6b 20 53 69 64 65 20 6f  66 20 74 68 65 20 4d 6f  |k Side of the Mo|
00000070  6f 6e 04 00 00 00 00 00  00 00 79 65 61 72 00 31  |on........year.1|
00000080  39 37 33 05 00 00 00 00  00 00 00 74 72 61 63 6b  |973........track|
00000090  00 30 34 2f 31 30 04 00  00 00 00 00 00 00 67 65  |.04/10........ge|
000000a0  6e 72 65 00 52 6f 63 6b  26 00 00 00 00 00 00 00  |nre.Rock&.......|
000000b0  63 6f 6d 6d 65 6e 74 00  43 61 70 69 74 61 6c 20  |comment.Capital |
000000c0  52 65 63 6f 72 64 73 20  2d 20 43 44 50 20 37 32  |Records - CDP 72|
000000d0  34 33 20 35 20 38 32 31  33 36 20 32 20 31 1b 00  |43 5 82136 2 1..|
000000e0  00 00 00 00 00 00 63 72  65 64 69 74 00 4d 61 73  |......credit.Mas|
000000f0  6f 6e 00 57 61 74 65 72  73 00 57 72 69 67 68 74  |on.Waters.Wright|
00000100  00 47 69 6c 6d 6f 75 72  41 50 45 54 41 47 45 58  |.GilmourAPETAGEX|
00000110  d0 07 00 00 08 01 00 00  08 00 00 00 00 00 00 c0  |................|
00000120  00 00 00 00 00 00 00 00                           |........|
00000128
```
So many unnecessary padding bytes :(


'gatepa' has a few other modes (use --help)


'gatepa' does not support renaming files, but we can accomplish that using
the 'extract' mode and some shell.
(I will leave that as an exercise for the reader.)


## History

I wrote a personal program for editing apetags a couple of years ago.
It was a decent learning experience; the program worked (mostly),
but let us say that the overall design and implementation was pretty rough.
So after finishing up ttaR(1), I wanted to rewrite that old program using some
stuff I had since learned like:


1 - arena allocation (no micro-allocations everywhere)

2 - custom string types (cedarDB's German strings)

3 - using references everywhere instead of always copying (easier with 1 & 2)

4 - pervasive integer overflow checking

5 - a unity build


old program: (using the DSotM example above)
```
total heap usage: 511 allocs, 240 frees, 69,777 bytes allocated
```
It frees everything in debug mode,
but all the micro-allocations and pointer chasing
can make the cleanup function time measurable,
especially with a larger number of files.


new program:
```
total heap usage: 9 allocs, 9 frees, 13,824 bytes allocated
```
Most of the memory here is actually unused;
The program uses 3 seperate arenas with 4K blocks + some overhead.
I did cheat a bit,
because the new program uses unbuffered i/o for reading/writing tags.
