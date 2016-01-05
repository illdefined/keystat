# keystat
Keyboard statistics generator

**keystat** is a c-programm which listens for keypresses on a input device and stores the time triplets take to be typed. This can be used to create a realistic cost model for keyboards, for example to optimise keyboard layouts.

## Installation
To install **keystat** simply run

```
make
sudo make install
```

## Usage
**keystat** comes equipped with a systemd-service. Just modify line 5 of `keystat.service`. You have to change `/dev/input/by-path/platform-i8042-serio-0-event-kbd` to the input path of your keyboard. To find yours look into `/dev/input/by-path/` and find the one ending with `-kdb`. If there is more than on file ending with `-kdb`, `cat` them. The one which changes when you type is the searched one.

## Output

`keystat` stores the data of the triples in the first filename you give it. This file can be read with `dump`. `dump` outputs the data tab-seperated. The first column consists of three numbers (space-seperated) which are the keycodes of the triple. The second column is one number, the times this triple was typed. The last column is the average time it took to type this triple in microseconds.
