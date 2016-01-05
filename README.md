# keystat
Keyboard statistics generator

**keystat** is a C programme to create realistic typing cost model that can be used to optimise keyboard layouts. It does so by recording the average time it takes to type a triplet of key scan codes.

## Installation
To install **keystat** simply run:

```
make
sudo make install
```

## Usage
**keystat** comes equipped with a systemd service unit that will work without configuration on most x86‐based computer platforms.

On other platforms or when your primary keyboard is attached via USB, the event device specified by the variable `KEYSTAT_DEV` in the environment file `/etc/default/keystat` may have to be changed. To find the correct one, take a look into `/dev/input/by-path/` and identify those ending in `-kbd`.

If there is more than one, `cat` them and see which of them generates output when you type on the keyborad.

## Output
`keystat-capture` stores the triplet data in the map file given as first command‐line argument. This file can be inspected with `keystat‐dump`, yielding a table of space‐separated values:

```
sudo keystat-dump /var/lib/keystat/map
```

The first three columns are the scan codes of the triplet. The fourth column gives the number of times this triplet was recorded and the last one the average time it took to type it.

For convenience, the `keystat-translate` tool translates the scan codes to symbolic names according to the Linux kernel headers.
Please keep in mind that these symbolic names do not represent the actual key mappings of your specific keyboard layout but those of a standard US keyboard.

```
sudo keystat-dump /var/lib/keystat/map | keystat-translate
```

The tool yields a tab-separated table with the first column being a space-separated triple of scan codes, the second the recorded number of occurrences and the third one the average time.
