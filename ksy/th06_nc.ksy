meta:
  id: th06_nc
  endian: le
seq:
  - id: mode
    type: u1
  - id: shot
    type: u1
  - id: difficulty
    type: u1
  - id: padding
    size: 3
  - id: unknown_1
    type: u4
  - id: unknown_2
    type: u4
  - id: date
    type: str
    size: 9
    encoding: ASCII
    terminator: 0x0
  - id: name
    type: str
    size: 9
    encoding: Shift_JIS
    terminator: 0x0
  - id: unknown_3
    type: u2
  - id: score
    type: u8
  - id: slowdown_2
    type: f4
  - id: slowdown
    type: f4
  - id: slowdown_3
    type: f4
  - id: padding2
    size: 4
  - id: stage_offsets
    type: stage_pointer
    repeat: expr
    repeat-expr: 7
types:
  stage_pointer:
    seq:
      - id: offset
        doc: Absolute offset to stage blocks
        type: u8
    instances:
      # See https://github.com/kaitai-io/kaitai_struct/issues/14
      # for an explanation of this pattern.
      stage_header:
        io: _root._io
        pos: offset
        type: stage_header
        size: 20
        if: offset != 0
      input_frames:
        io: _root._io
        pos: offset + 20
        type: input_frame
        size: 12
        repeat: until
        repeat-until: _.frame_num == 9999999
  stage_header:
    seq:
      - id: score
        type: u8
      - id: seed
        type: u2
      - id: unknown_1
        type: u2
      - id: power
        type: u1
      - id: lives
        type: s1
      - id: bombs
        type: s1
      - id: unknown_2
        size: 3
      - id: misses
        type: s2
  input_frame:
    seq:
      - id: frame_num
        type: s4
      - id: input
        type: u8
