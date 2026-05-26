#!/bin/sh

rm -f tmp.h

printf "static const std::array<SDL_Color, MAX_HEIGHT> height_to_colormap = {{\n" >> tmp.h
python3 script.py < gen_colorange.txt | head -n 256 >> tmp.h
printf "}};\n" >> tmp.h

mv tmp.h include/colorange.h

