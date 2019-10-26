#!/usr/bin/env bash
mkdir pack
cp main.cpp pack/
cp node.out pack/
cd pack
tar zcvf chain_golf_hot_potatoes.tar.gz *
cd ..
mv pack/chain_golf_hot_potatoes.tar.gz .
rm -rf pack

