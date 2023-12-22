import sys
from hashlib import sha256
from getpass import getpass
from base64 import a85encode

ENCODING = 'utf-8'

try:
  ctx = sha256()
  args = sys.argv[1:]
  master = getpass('Master Password: ')

  ctx.update(master.encode(ENCODING))
  checksum = ctx.hexdigest()[0:8]
  [ctx.update(param.encode(ENCODING)) for param in args]
  password = a85encode(ctx.digest()).decode(ENCODING)

  sys.stderr.write(f'Checksum: {checksum}\n')
  sys.stdout.write(password)
except KeyboardInterrupt:
  sys.stderr.write('Aborting.\n')
