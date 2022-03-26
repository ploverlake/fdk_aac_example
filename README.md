# fdk_aac_example

* Latest [fdk-aac](https://github.com/mstorsjo/fdk-aac)
* Latest [mp4v2](https://github.com/enzo1982/mp4v2)

## Clone

```bash
$ git clone https://github.com/ploverlake/fdk_aac_example.git

$ cd /path/to/fdk_aac_example
$ git submodule update --init --recursive
```

## Build

```bash
$ cd /path/to/fdk_aac_example
$ ./build.sh
```

## Test

```bash
$ cd /path/to/fdk_aac_example

# ADTS
$ ./run_enc_adts.sh -a 29 /path/to/XXX.wav

# m4a
$ ./run_enc_m4a.sh -a 39 /path/to/XXX.wav
```
