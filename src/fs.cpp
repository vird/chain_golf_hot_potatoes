#include <sys/stat.h>
// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c/19841704
bool file_exists(string& name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0); 
}

// some extra durability
bool file_save(string& path, u8* buf, u32 len) {
  FILE* fh = fopen(path.c_str(), "wb");
  if (!fh) return false;
  fwrite(buf, 1, len, fh);
  if (ferror(fh)) return false;
  fclose(fh);
  if (ferror(fh)) return false;
  return true;
}
bool file_load(string& path, u8* buf, u32 len) {
  FILE* fh = fopen(path.c_str(), "rb");
  if (!fh) return false;
  fread(buf, 1, len, fh);
  if (ferror(fh)) return false;
  fclose(fh);
  if (ferror(fh)) return false;
  return true;
}