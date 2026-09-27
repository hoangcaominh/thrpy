meta:
  id: th06
  file-extension: raw
  endian: le
  bit-endian: le
  imports:
    - th06_o
    - th06_c
    - th06_nc
seq:
  - id: magic
    contents: T6RP
  - id: version
    type: u2
  - id: body
    type:
      switch-on: version
      cases:
        0x0102: th06_o
        0x0103: th06_c
        0x010B: th06_nc
        0x010F: th06_nc
