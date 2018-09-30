# Chat Server

## Compiled on Linux Ubuntu

### Server

```
    g++ server.cpp api/api.cpp -o server
```

### Client

```
    g++ -pthread client.cpp -o client
```

## Run

### Server

Before running:

```
    sudo apt install fortune-mod
```

Then:

```
    ./server
```

### Client

```
    ./client localhost 23001 23002 23003
```
