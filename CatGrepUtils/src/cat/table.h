#ifndef TABLE_H
#define TABLE_H

void CatSetTable(const char *table[static 256]);
void CatEndl(const char *table[static 256]);
void CatTabs(const char *table[static 256]);
void CatUnprintable(const char *table[static 256]);

#endif