import sys
from hashlib import sha256
from getpass import getpass
from base64 import a85encode

ENCODING = 'utf-8'

try:
  ctx = sha256()
  args = (arg.encode(ENCODING) for arg in sys.argv[1:])
  master = getpass('Master Password: ').encode(ENCODING)

  ctx.update(master + b'\n')
  checksum = ctx.hexdigest()[0:8]
  ctx.update(b'\n'.join(args) + b'\n')
  password = a85encode(ctx.digest())

  sys.stderr.write(f'Checksum: {checksum}\n')
  sys.stdout.write(password.decode(ENCODING))
except KeyboardInterrupt:
  sys.stderr.write('Aborting.\n')
