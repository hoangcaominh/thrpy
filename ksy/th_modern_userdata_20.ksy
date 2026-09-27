meta:
  id: th_modern_userdata_20
  endian: le
seq:
  - id: magic
    contents: USER
  - id: len_userdata
    type: u4
  - id: unknown
    size: 4
  - id: user_desc
    type: str
    terminator: 0xD
    encoding: Shift_JIS
  - id: user_desc_term
    type: str
    terminator: 0xA
    encoding: Shift_JIS
  - id: version
    type: userdata_field
  - id: name
    type: userdata_field
  - id: date
    type: userdata_field
  - id: shot
    type: userdata_field
  - id: difficulty
    type: userdata_field
  - id: stage
    type: userdata_field
  - id: score
    type: userdata_field
  - id: slowdown
    type: userdata_field
types:
  userdata_field:
    seq:
      - id: value
        type: str
        terminator: 0xD
        encoding: UTF-8
      - id: term
        type: str
        terminator: 0xA
        encoding: UTF-8
