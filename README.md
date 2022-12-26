# Lempel-Ziv-Welch compression algorithm

This is an implementation in C99 of the LZW compression algorithm.

## Build

```console
$ gcc -O1 compress.c -o compress
$ gcc -O1 decompress.c -o decompress
```

## Compress a file

Currently when running the `compress` binary file from the command line the application will compress the file `input.txt` present in the folder and, as a result, you will get the file `compressed.dat`. Those filenames are hardcoded in the source code, so I encourage you to modify the code to allow pass an input and output filename as command line arguments.

```console
$ ./compress
```

## Decompress a file

In order to decompress data you should run the `decompress` binary file from the command line. This application uses the file named `compressed.dat` as the input file and `decompressed.dat` as the output file. As I said before, I strongly encourage you to modify the code to allow pass an input and output filename from the command line.

```console
$ ./decompress
```

Please, feel free to submit your Pull Requests. Your contributions are welcome!

## Development tutorial

You can watch a technical very detailed video tutorial I made explaining step-by-step all the processes and phases involved during the development of this code. I hope you find it really useful for your learning purposes.

[![How to implement the Lempel–Ziv–Welch compression algorithm in C99](https://img.youtube.com/vi/MbxvZiuFO2U/0.jpg)](https://www.youtube.com/watch?v=MbxvZiuFO2U)