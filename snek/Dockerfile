# Use an official lightweight C runtime as a parent image
FROM gcc:latest

# Set the working directory in the container to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Make port 80 available to the world outside this container
EXPOSE 49777

# Compile the C program
RUN gcc -o snek.out snek.c
