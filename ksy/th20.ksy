meta:
  id: th20
  file-extension: raw
  endian: le
  imports:
    - th_modern_header_20
    - th_modern_userdata_20
seq:
  - id: magic
    contents: t20r
  - id: version
    type: u4
  - id: th_modern_header
    type: th_modern_header_20
  - id: name
    type: str
    size: 16
    terminator: 0
    encoding: Shift_JIS
  - id: timestamp
    type: u8
  - id: score
    type: u4
  - id: unknown_1
    size: 180
  - id: slowdown
    type: f4
  - id: num_stages
    type: u4
  - id: shot  # 0 is Reimu, 1 is Marisa
    type: u4
  # 0 red, 1 red2, 2 blue, 3 blue2, 4 yellow, 5 yellow2, 6 green, 7 green2, 8 default (sub only)
  # this does seem to be correct
  # note that the first stone is the "main" stone
  - id: stones
    type: u4
    repeat: expr
    repeat-expr: 4
  - id: unknown_2
    size: 4
  - id: difficulty
    type: u4
  # Win on stage 3, this is 3. Die on stage 1, this is 1.
  # So probably related to num_stages or stage practice
  - id: unknown_3
    size: 4
  - id: unknown_4  # Always 00000000
    size: 4
  # No spell practice in the trial, so we are just guessing
  - id: spell_practice_id
    type: u4
instances:
  userdata:
    type: th_modern_userdata_20
    pos: th_modern_header.userdata_offset
# The stage format remains unknown.
