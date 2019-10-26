# What is it

Simple cryptocurrency with codegolf minimizer for hackathon.

# How to install
## Dependencies

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

# Credits
### used libraries

  * https://github.com/orlp/ed25519
    * digital signature
    * sha2
  * for lower size precompute was replaced with https://gist.github.com/irfansehic/9c0b204845370f372bdfccfb66ec5942
