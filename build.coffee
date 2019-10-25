#!/usr/bin/env iced
require 'fy'
fs = require 'fs'
{execSync} = require 'child_process'
src_path = "src"
# ###################################################################################################
#    utils
# ###################################################################################################
unpack_include = (code)->
  while reg_ret = /\#include \"([^\"]*)\"/.exec code
    [_skip, file] = reg_ret
    p "unfold #include \"#{file}\""
    code = code.replace "#include \"#{file}\"", fs.readFileSync "#{src_path}/#{file}", 'utf-8'
  code

strip_comments = (code)->
  code = code.replace /\n\/\/.*/g, ''

strip_empty_lines = (code)->
  code = code.replace /\n{2,}/g, '\n'

strip_token_and_new_line = (code)->
  # dont squash with #define
  code = code.replace /\s*([;{}\(\)])[\n\s]*(?!\#)/g, '$1'

strip_operator_space = (code)->
  code = code.replace /\s*([=,:]|<<)\s*/g, '$1'

str2tok = (str)->
  ret = []
  cur_tok = null
  rst = ()->
    cur_tok = {
      type : 'raw'
      code : ''
    }
  rst()
  default_fn = ()->
    cur_tok.code += str[0]
    str = str.slice 1
    return
  # ineffective as hell
  while str.length
    switch str[0]
      when '#'
        if !/\#include/.test str
          default_fn()
          break
        ret.push cur_tok
        [include_token] = /#include.*/.exec str
        ret.push {
          type: 'include'
          code: include_token
        }
        str = str.slice include_token.length
        rst()
        
      when '"'
        ret.push cur_tok
        # TODO escaping
        [str_token] = /\"[^\"]*\"/.exec str
        ret.push {
          type: 'string'
          code: str_token
        }
        str = str.slice str_token.length
        rst()
      else
        default_fn()
  ret.push cur_tok
  ret
  
tok2str = (list)->
  ret = []
  for v in list
    ret.push v.code
  ret.join ''

tok_map = (list, fn)->
  for v in list
    continue if v.type == 'string'
    continue if v.type == 'include'
    v.code = fn v.code
  return
# ###################################################################################################
#    proposition utils
# ###################################################################################################

prop_hash = {}
prop_fill = (str)->
  prop_regex = /(.)(\w+)/g
  while reg_ret = prop_regex.exec str
    [_skip, ch, word] = reg_ret
    continue if ch == "#"
    prop_hash[word] ?= 0
    prop_hash[word]++
  str

prop_print = ()->
  kv_list = []
  for k,v of prop_hash
    continue if v <= 1
    kv_list.push {k,v}

  kv_list.sort (a,b)->a.v-b.v
  for kv in kv_list
    {k,v} = kv
    potential_win = 0
    potential_win_extra = -"#define AA #{k}".length
    potential_win += v*(k.length - 2)
    continue if potential_win <= 0
    p [
      k.rjust 20
      "=>"
      v.rjust 3
      potential_win.rjust 5
      (potential_win+potential_win_extra).rjust 5
    ].join ' '
  return
# ###################################################################################################

main = fs.readFileSync "#{src_path}/main.cpp", 'utf-8'
main = unpack_include main

tok_list = str2tok main
tok_map tok_list, strip_comments
tok_map tok_list, strip_empty_lines
tok_map tok_list, strip_token_and_new_line
tok_map tok_list, strip_operator_space

# ###################################################################################################
#    TODO autopacker here
# ###################################################################################################
###
// type related
#define I int
#define S string
#define C const
// server related
#define sVt serverVersion_t
#define bAAM bindAndAddMethod
// JSON
#define PBN PARAMS_BY_NAME
#define JI JSON_INTEGER
#define JS JSON_STRING
#define V Value
#define aS asString
// control flow and structures
#define P public
#define R return


// blockchain related
// from_address = fA
// to_address = tA
// address = A
// amount = w (wei)
// balance = B
// transaction = T
// transactionI = T
###

replace_map = {}
hard_replace_map = {}
soft_replace = (k, v)->
  replace_map[k] = v
hard_replace = (k, v)->
  replace_map[k] = v
  hard_replace_map[k] = true

# type related
soft_replace "const", "C"
soft_replace "string", "S"
# server related
hard_replace "MyServer", "MS"
hard_replace "TemplateServer", "TS"
soft_replace "AbstractServer", "AS"
soft_replace "AbstractServerConnector", "ASC"
hard_replace "request", "rq"
hard_replace "response", "rs"
hard_replace "connector", "c"
# soft_replace "bindAndAddMethod", "bAAM"
soft_replace "bindAndAddMethod", "bM"
soft_replace "serverVersion_t", "sVt"
hard_replace "httpserver", "hs"
# JSON
soft_replace "JSON_STRING", "JS"
# reserved words
soft_replace "virtual", "V"
hard_replace "NULL", "0"
soft_replace "using namespace", "UN"

# blockchain related
hard_replace "to_address", "tA"
hard_replace "from_address", "fA"
hard_replace "transaction", "tx"
hard_replace "address", "A"
hard_replace "balance", "B"

for k,v of replace_map
  tok_map tok_list, (str)->
    if /\s/.test k
      regex = new RegExp(RegExp.escape(k), "g")
    else
      regex = new RegExp("\\b"+RegExp.escape(k)+"\\b", "g")
    str = str.replace regex, v

jl = []
for k,v of replace_map
  continue if hard_replace_map[k]
  jl.push "#define #{v} #{k}\n"

# ###################################################################################################
#    auto propositions
# ###################################################################################################

tok_map tok_list, prop_fill
prop_print()
# INJECT delayed, because wasting stats
tok_list.unshift {
  type : 'raw'
  code : jl.join ''
}

main = tok2str tok_list


# TODO replace file name
fs.writeFileSync "main.cpp", main
execSync "./build.sh"