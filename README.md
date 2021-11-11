# ft_printf-tester
A simple tester for 42's ft_printf project.

**Note**: this was written in November 2021. It might be that this version becomes outdated in the future, as I have no intention to update it myself.
Also, I am not sure if it checks every case which needs to be checked. This tester isn't perfect; I wrote it so I could test my own printf which I was working on, and it helped me, and I am sharing it with the world.


## Features
 - fast
 - return value checking
 - bonus
 - does not check undefined behaviour
 - if your printf segfaults, the tester segfaults.

## Installing
just clone this repo in your printf folder.
```git clone https://github.com/joppiesaus/ft_printf-tester;
cd ft_printf-tester
```
May you want it somewhere else, just change `FT_PRINTF_PATH` in the `Makefile`.

## Running
`make` and run `test`. When you change your printf, you must `make` again.
```
make; ./test
```

For bonus:
```
make; ./test b
```
