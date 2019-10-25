#!/usr/bin/env iced
require 'fy'
fs = require 'fs'
{execSync} = require 'child_process'
argv = require('minimist')(process.argv.slice(2))
argv.pack ?= true
# ###################################################################################################
#    utils
# ###################################################################################################
file_loaded_hash = {}
unpack_include = (code)->
  while reg_ret = /\#include \"([^\"]*)\"/.exec code
    [_skip, file] = reg_ret
    if !file_loaded_hash[file]
      file_loaded_hash[file] = true
      puts "unfold #include \"#{file}\""
      res = fs.readFileSync file, 'utf-8'
    else
      puts "SKIP #include \"#{file}\""
      res = ""
    code = code.replace "#include \"#{file}\"", res
  code

compact_preprocessor = (code)->
  code = code.replace /\\\n/g, ''

strip_comments = (code)->
  code = code.replace /\/\*((?!\*\/)[\s\S])+\*\//g, ''
  code = code.replace /\n\s*\/\/.*/g, ''

strip_empty_lines = (code)->
  code = code.replace /\n{2,}/g, '\n'

strip_token_and_new_line = (code)->
  # code = code.replace /\s*([;{}\(\)])[\n\s]*(?!\#)/g, '$1'
  code = code.replace /\s*([;{}\(\)])[\n\s]*/g, '$1'

# note #define is in another token, so you can't reach it with strip_token_and_new_line, so need fix after
fix_semicolon_preproc = (code)->
  code = code.replace /([;{}\(\)])\#/g, '$1\n#'
  

strip_operator_space = (code)->
  code = code.replace /\s*([-=,:*+^]|<<|>>)\s*/g, '$1'

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
  eat_regex = (type, regex, join=false)->
    if !join
      ret.push cur_tok
    [token] = regex.exec str
    str = str.slice token.length
    if !join
      ret.push {
        type
        code: token
      }
      rst()
    else
      cur_tok.code += token
    return
  # ineffective as hell
  while str.length
    switch str[0]
      when '/'
        switch str[1]
          when "/" # //
            eat_regex 'raw', /^\/\/.*/, true
          when "*" # /*
            eat_regex 'raw', /^\/\*((?!\*\/)[\s\S])+\*\//, true
          else
            default_fn()
      when '#'
        if !reg_ret = /^\#(include|define|if|ifdef|ifndef|endif).*/.exec str
          default_fn()
          break
        eat_regex 'include', /^\#(include|define|if|ifdef|ifndef|endif).*/
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

_prop_print = (letter_count)->
  kv_list = []
  for k,v of prop_hash
    continue if v <= 2
    kv_list.push {k,v}

  kv_list.sort (a,b)->a.v-b.v
  puts "#{letter_count} letter"
  for kv in kv_list
    {k,v} = kv
    potential_win = 0
    potential_win_extra = -"#define  #{k}\n".length-letter_count
    potential_win += v*(k.length - letter_count)
    continue if potential_win <= 0
    puts [
      k.rjust 30
      "=>"
      v.rjust 3
      potential_win.rjust 5
      (potential_win+potential_win_extra).rjust 5
    ].join ' '
prop_print = ()->
  _prop_print 1
  _prop_print 2
  return
# ###################################################################################################

process.chdir 'src'
main = fs.readFileSync "main.cpp", 'utf-8'
main = unpack_include main
main = compact_preprocessor main
process.chdir '..'

tok_list = str2tok main
if argv.pack
  tok_map tok_list, strip_comments
  tok_map tok_list, strip_empty_lines
  tok_map tok_list, strip_operator_space
  tok_map tok_list, strip_token_and_new_line

# ###################################################################################################
#    TODO autopacker here
# ###################################################################################################
###
// type related
#define I int
// JSON
#define PBN PARAMS_BY_NAME
#define JI JSON_INTEGER
#define V Value
#define aS asString
// control flow and structures
#define P public
#define R return
// blockchain related
// amount = w (wei)
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
# soft_replace "string", "S"
###
#define u64 unsigned long long
#define i64 long long
soft_replace "i64", "I"
soft_replace "i32", "Q"
###
soft_replace "i64", "I"
soft_replace "i32", "Q"
# server related
hard_replace "MyServer", "MS"
hard_replace "TemplateServer", "TS"
soft_replace "AbstractServer", "AS"
soft_replace "AbstractServerConnector", "ASC"
hard_replace "request", "rq"
hard_replace "response", "rs"
hard_replace "connector", "c"
hard_replace "conn", "c"
# soft_replace "bindAndAddMethod", "bAAM"
soft_replace "bindAndAddMethod", "bM"
soft_replace "serverVersion_t", "sVt"
hard_replace "httpserver", "hs"
# JSON
soft_replace "JSON_STRING", "JS"
# reserved words
soft_replace "virtual", "V"
hard_replace "NULL", "0"
# soft_replace "using namespace", "UN"
soft_replace "using namespace", "U" # SHORTER TMP

# blockchain related
hard_replace "to_address", "tA"
hard_replace "from_address", "fA"
hard_replace "transaction", "tx"
hard_replace "transactionI", "tx"
hard_replace "address", "A"
hard_replace "balance", "B"
hard_replace "balanceI", "B"

if argv.pack
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

if argv.advice
  tok_map tok_list, prop_fill
  prop_print()
# INJECT delayed, because wasting stats
tok_list.unshift {
  type : 'raw'
  code : jl.join ''
}

main = tok2str tok_list
if argv.pack
  main = fix_semicolon_preproc main


# TODO replace file name
fs.writeFileSync "main.cpp", main
execSync "./build.sh"