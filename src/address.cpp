// suboptimal IDK
bool address_json_parse(Value t, u32 &res) {
  const char* tmp = t.asString().c_str();
  char* tmp_p;
  const char* end = tmp + strlen(tmp);
  res = strtol(tmp, &tmp_p, 10);
  return tmp_p == end;
}