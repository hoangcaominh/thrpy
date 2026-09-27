meta:
  id: th10
  file-extension: raw
  endian: le
  imports:
    - th_modern_header
    - th_modern_userdata
seq:
  - id: magic
    contents: t10r
  - id: version
    type: u4
  - id: th_modern_header
    type: th_modern_header
  - id: name
    type: str
    size: 12
    encoding: Shift_JIS
  - id: timestamp
    type: u4
  - id: score
    type: u4
  - id: unknown_1
    size: 52
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
    type: u4
  - id: unknown_4
    type: u4
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
      - id: unknown_1
        type: u2
      - id: unknown_2
        type: u4
      - id: len_stage_data
        type: u4
        doc: add to current stage offset, + current stage header length which is 0x1c4
      - id: score
        type: u4
      - id: power
        type: u4
      - id: piv
        type: u4
      - id: unknown_3
        type: u4
      - id: lives
        type: u4
      - id: rest_of_header
        size: 420
      - id: stage_data
        size: len_stage_data
