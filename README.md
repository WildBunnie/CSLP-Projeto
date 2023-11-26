# CSLP - Project: Image and Video compression 

Gorup 2:
* Bárbara Nóbrega Galiza - 105937
* João Miguel Dias Andrade - 107969
* Tomás Victal - 109018

This project is organized in diferent directories, where each one represents an entity or deliverable resolution. To test the code implemented, is necessary to do it inside the choosen directory.

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
