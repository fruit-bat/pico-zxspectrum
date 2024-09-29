#!/bin/bash
mkdir -p temp_uf2
rm -rf temp_uf2/*
find build-* -name '*.uf2' -execdir bash -c "cat {} >> $PWD/temp_uf2/{}" \;

