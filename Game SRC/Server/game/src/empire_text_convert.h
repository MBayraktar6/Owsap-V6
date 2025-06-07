#ifndef __INC_EMPIRE_CHAT_CONVERT_H__
#define __INC_EMPIRE_CHAT_CONVERT_H__

bool LoadEmpireTextConvertTable(DWORD dwEmpireID, const char* c_szFileName);
void ConvertEmpireText(DWORD dwEmpireID, char* szText, size_t len, int iPct);

#endif // __INC_EMPIRE_CHAT_CONVERT_H__
