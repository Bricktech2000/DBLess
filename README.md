# DBLess

_A hash-based, database-less password manager_

## Overview

This repository contains a few implementations of the DBLess algorithm, from which may be built a password manager that does not require encryption or password storage.

The DBLess algorithm is an embellished hash function, consisting of updating a SHA‑256 hash object with a master password then repeatedly with an argument sequence before encoding the resulting digest in Ascii85:

```python
def dbless(master, *args): return a85encode(sha256(b'\n'.join([master, *args, b''])).digest()).replace(b'z', b'!' * 5)
```

The argument sequence `args` is a list of strings that uniquely identifies the account to be logged into; examples of such sequences include `['github.com', 'email@provider']` and `[token, 'cloudflare', 'us-east-1', 'root']` where `token` would be a secret token stored in a local file or environment variable.

## Usage

```sh
# C implementation
make bin/dbless && bin/dbless [args...] # replace `[args...]` with an argument sequence

# Python implementation
python3 src/dbless.py [args...] # replace `[args...]` with an argument sequence
```

The implementations within this repository are meant to be wrapped by a shell alias:

```sh
# do not load token, then output password to stdout
dbless() { bin/dbless $@; }

# load token from environment variable, then output password to stdout
dbless() { bin/dbless $TOKEN $@; }

# load token from environment variable, then output password to clipboard
dbless() { bin/dbless $TOKEN $@ | xclip -selection clipboard; }

# load token from local file, then output password to clipboard
dbless() { bin/dbless $(cat token) $@ | xclip -selection clipboard; }
```
