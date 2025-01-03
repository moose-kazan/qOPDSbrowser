# qOPDSbrowser #

![Screenshot](./screenshot.png)

## About ##

Simple OPDS browser. Written with C++/Qt.

## Install from Ubuntu PPA ###

```
sudo add-apt-repository ppa:bulvinkl/ppa
sudo apt update
sudo apt install qopdsbrowser
```

## Install from source on Debian 12 and Ubuntu 22.04 ##

Install dependencies:

```
apt install git cmake build-essential qt6-base-dev qt6-tools-dev
```

Clone repository:

```
git clone https://github.com/moose-kazan/qOPDSbrowser
```


Change directory:

```
cd qOPDSbrowser
```


Build it with CMake:

```
mkdir -p build && cd build && cmake .. && make
```

Run:

```
./qOPDSbrowser
```


## Contacts ##
Vadim V. Kalinnikov <moose@ylsoftware.com>


