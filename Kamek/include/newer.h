#ifndef NEWER_H
#define NEWER_H 

extern int lastLevelIDs[];


int ConvertEnglishToJapanese(wchar_t *output, int world, int level, int outputLength);
void ConvertEnglishToJapanese(wchar_t *output, int world, int level);
void WriteJPWorldNameToTextBox(nw4r::lyt::TextBox *tb, int world);
void GetJapaneseWorldName(wchar_t *output, int world);

void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source);
void WriteLevelNameToTextBox(nw4r::lyt::TextBox *tb, int world, int level);

void WriteWorldNameToTextBox(nw4r::lyt::TextBox *tb, int world);

int getUnspentStarCoinCount();
int getStarCoinCount();
void getNewerLevelNumberString(int world, int level, wchar_t *dest);



#endif /* NEWER_H */
