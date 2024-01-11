# CSLP - Project: Image and Video compression 

Gorup 2:
* Bárbara Nóbrega Galiza - 105937
* João Miguel Dias Andrade - 107969
* Tomás Victal - 109018

This project is organized in diferent directories, where each one represents an entity or deliverable resolution. To test the code implemented, is necessary to do it inside the choosen directory.

## OBS: Deliverable 6 updated files are all and only on "projeto_final" directory
## OBS2: Script to run the dlv6: main

# Compile
While on `src/<choosen-dir>`:

`cmake .`

`make`

# How to run
## videoPlayer:
`./bin/player <video> [filter] `

### Filter options:
- gaussian \[ksize] \[sigma]
- blur \[ksize]
- watermark \<watermark> \[x] \[y] \[alpha]
- grayscale 
- threshold \[treshold]
- rgb2yuv
- yuv2rgb
- colorhisteq
- grayhisteq
- histogram

## BitStream, GolombCoding, hibrid-encoder, intraEncoder, hibrid-encoder:
`./bin/main `

## projeto_final
`./bin/main [inputfile] [option]`

### Options:
  - h    ->prints this message
  - g:16 ->golomb value
  - b:32 ->block size
  - s:3  ->search area
  - p:6  ->periodicity
  - q:-1  -> quantization
  - m:e  ->mode encode(e), decode(d)
  - e:hybrid ->encoding type [hybrid,intra,lossy]
  - o:outputfile ->output file
  - t:No default ->test mode 0-lossless 1-lossy
