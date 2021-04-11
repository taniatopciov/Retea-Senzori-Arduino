#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include <SPI.h>
#include <SD.h>

class SDCardManager
{
public:
    SDCardManager();
    ~SDCardManager();

    bool InitSDCard();
    bool IsConnected() const { return isConnected_b; }

    bool OpenFile(char *filePath);
    bool OpenFileForWrite(char *filePath, bool truncate = false);
    bool HasData();
    bool CreateFile(char *filePath);
    bool CreateFolder(char *folderPath);
    bool ExistsFile(char *filePath);
    void CloseFile();

    bool WriteRawByte(char byte);
    bool WriteRawBytes(char *bytes, size_t size);
    bool ReadRawBytes(char *bytes, size_t size);

    void CardInfo();
    void ListFiles();
    void DumpFile(char *filePath);

private:
    void PrintDirectory(File dir, int numTabs);

    File openedFile;
    bool isConnected_b;
    bool isFileOpened_b;
};

extern SDCardManager g_SDCardManager;

#endif // SD_CARD_MANAGER_H