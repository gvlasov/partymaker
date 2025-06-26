# partymaker

"Make your own continuous background noise" CLI kit for Linux, ALSA-based

## Build

```sh
make
```

## Commands

```sh
./tone FREQUENCY_HZ[xAMPLITUDE] ... - generates a sum of sine waves defined by their frequencies and amplitude factor
./tone 120       # Single 120hz wave
./tone 120 200   # Multiple waves
./tone 120x2 200 # Increase volume of a single wave
./tone 30 31 32 33 34 35 36.4 37.1 # Doom cat purring

./attack - generates a repeating clear tone
./attack FREQUENCY_HZ DURATION_MS PERIOD_MS
./attack 2000 100 1000 # Timer ticking
./attack 200 1000 1000 # Fasten your seatbelts, we're taking off
./attack 80 9000 9000  # Grandpa's clock

./djent - attack, but kinda saw instead of clear
./djent FREQUENCY_HZ DURATION_MS PERIOD_MS

./noise - noise
./noise DB_DELTA_PER_OCTAVE
./noise 0  # White noise
./noise -3 # Pink noise
./noise -6 # Brown noise

./brown - more clear brown noise
```

## Composite examples

```sh
sounds/focus
```

