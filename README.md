# What is it

Simple cryptocurrency with codegolf minimizer for hackathon.

## Features

  * ed25519 signature
  * sha512 hash
  * semi-automatic code minifier
  * -40kb (in packed) ed25519 codebase with replacing precomputed table with runtime ones

# How to install
## Dependencies
It's better to start from Ubuntu 18.04.3

    git clone https://github.com/vird/chain_golf_hot_potatoes
    cd chain_golf_hot_potatoes
    ./prepare-env.sh

## How to run

    ./run.sh

## Example RPC calls

     ./curl_test.sh

# Development
## Dependencies

    curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.0/install.sh | bash
    source ~/.bashrc
    nvm i 12
    npm i -g iced-coffee-script
    npm i

## How to preprocess + minify + build

    ./build.coffee

## How to test in docker
**under construction**

    # ensure your build is up to date
    ./run_test.sh

# Credits
### Used libraries

  * https://github.com/orlp/ed25519
    * digital signature
    * sha2
  * for lower size precompute was replaced with https://gist.github.com/irfansehic/9c0b204845370f372bdfccfb66ec5942
