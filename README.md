# DBLess

_A hash-based, database-less password manager_

## Overview

This repository contains a few implementations of the DBLess algorithm, from wihch may be built a password manager that does not require encryption or password storage.

The DBLess algorithm is an embellished hash function which consists of updating a SHA256 hash object with a master password then repeatedly with an argument sequence before encoding the resulting digest in Ascii85:

```python
def dbless(master, *args): return a85encode(sha256(b'\n'.join([master, *args, b''])).digest())
```

The argument sequence `args` is a list of strings that must uniquely identify the service to be logged into; examples of such a sequence would be `['github.com', 'email@provider']` and `[token, 'cloudflare', 'us-east-1', 'root']` where `token` is a secret token stored in a local file or environment variable.

## Usage

```bash
# C implementation
make && bin/dbless [args] # replace `[args]` with an argument sequence

# Python implementation
python3 src/dbless.py [args] # replace `[args]` with an argument sequence
```

The implementations within this repository are meant to be wrapped by a shell alias:

```fish
# do not load token, then output password to stdout
function dbless; bin/dbless $argv; end

# load token from environment variable, then output password to stdout
function dbless; bin/dbless $TOKEN $argv; end

# load token from environment variable, then output password to clipboard
function dbless; bin/dbless $TOKEN $argv | xclip -selection clipboard; end

# load token from local file, then output password to clipboard
function dbless; bin/dbless (cat token) $argv | xclip -selection clipboard; end
```
