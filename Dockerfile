# Get the base Ubuntu image from Docker Hub
FROM ubuntu:latest

# Update apps on the base image
RUN apt-get -y update && apt-get install -y

# Install the Clang compiler
RUN apt-get -y install clang make cmake

# Copy the current folder which contains C++ source code to the Docker image under /usr/src
COPY . /usr/src/SystemMonitor

# Specify the working directory
WORKDIR /usr/src/SystemMonitor

#libncurses5-dev libncursesw5-dev
RUN make clean
RUN make build