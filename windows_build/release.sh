#!/bin/bash

DLLS=./*.dll
GAME=blaster.exe
INI=options.ini
LICENSES=./*.txt

OUTPUT=blaster.zip

zip $OUTPUT $DLLS $GAME $INI $LICENSES

