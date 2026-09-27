meta:
  id: th11
  file-extension: raw
  endian: le
  imports:
    - th_modern_header
    - th_modern_userdata
seq:
  - id: magic
    contents: t11r
  - id: version
    type: u4
  - id: th_modern_header
    type: th_modern_header
  - id: name
    type: str
    size: 12
    terminator: 0x0
    encoding: Shift_JIS
  - id: timestamp
    type: u8
  - id: score
    type: u4
  - id: unknown_1
    size: 60
  - id: slowdown
    type: f4
  - id: num_stages
    type: u4
  - id: shot
    type: u4
  - id: subshot
    type: u4
  - id: difficulty
    type: u4
  - id: unknown_3
    size: 8
  - id: stages
    type: stage
    repeat: expr
    repeat-expr: num_stages
instances:
  userdata:
    type: th_modern_userdata
    pos: th_modern_header.userdata_offset
types:
  stage:
    seq:
      - id: stage_num
        type: u2
      - id: seed
        type: u2
      - id: ignore
        type: u4
      - id: len_stage_data
        type: u4
      - id: score
        type: u4
      - id: power
        type: u4
      - id: piv
        type: u4
      - id: lives
        type: u2
      - id: life_pieces
        type: u2
      - id: unknown_1
        size: 24
      - id: graze
        type: u4
      - id: unknown_2
        size: 88
      - id: stage_data
        size: len_stage_data
