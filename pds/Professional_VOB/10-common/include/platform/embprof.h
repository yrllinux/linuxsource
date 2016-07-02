#ifndef EMBPROF_H
#define EMBPROF_H

extern void prof_init(void);
extern void prof_fini(void);
extern void prof_dump(void);

extern void prof_begin(prof_index_e i);
extern void prof_end(prof_index_e i);

#endif //EMBPROF_H
