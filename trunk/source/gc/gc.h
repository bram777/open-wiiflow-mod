#ifdef __cplusplus
extern "C"
{
#endif

#ifndef GC_H_
#define GC_H_
void set_video_mode(int i);
void set_language(u8 lang);
void DML_RemoveGame(const char *discid);
bool DML_GameIsInstalled(char *discid);
#endif //GC_H_

#ifdef __cplusplus
}
#endif