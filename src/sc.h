/*
The set of scalars is \Z/l
where l = 2^252 + 27742317777372353535851937790883648493.
*/

void sc_reduce(u8 *s);
void sc_muladd(u8 *s, const u8 *a, const u8 *b, const u8 *c);

