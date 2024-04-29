# Use an official lightweight C runtime as a parent image
FROM debian:stretch-slim

# Set the working directory in the container to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install any needed packages specified in requirements.txt
RUN apt-get update && \
    apt-get install -y gcc && \
    rm -rf /var/lib/apt/lists/*

# Make port 80 available to the world outside this container
EXPOSE 80

# Compile the C program
RUN gcc -o snek snek.c

# Run snek when the container launches
CMD ["./snek"]