#include "SDCardManager.h"
#include <string.h>

#define PIN_SD_CARD_CS 10

SDCardManager::SDCardManager()
{
    isConnected_b = false;
}

SDCardManager::~SDCardManager()
{
}

bool SDCardManager::InitSDCard()
{
    if (!SD.begin(PIN_SD_CARD_CS, PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK))
    {
        isConnected_b = false;
        Serial.println("SD CARD init fail");
        return false;
    }

    isConnected_b = true;
    return true;
}

bool SDCardManager::OpenFile(char *filePath)
{
    if (!isConnected_b)
    {
        return false;
    }

    if (!ExistsFile(filePath))
    {
        return false;
    }

    if (isFileOpened_b)
    {
        CloseFile();
    }

    openedFile = SD.open(filePath);

    if (openedFile)
    {
        isFileOpened_b = true;
    }
    else
    {
        isFileOpened_b = false;
    }

    return isFileOpened_b;
}

bool SDCardManager::OpenFileForWrite(char *filePath, bool truncate)
{
    if (!isConnected_b)
    {
        return false;
    }

    if (isFileOpened_b)
    {
        CloseFile();
    }

    uint8_t mode_u8 = FILE_WRITE;
    if (truncate)
    {
        mode_u8 |= O_TRUNC;
    }

    openedFile = SD.open(filePath, mode_u8);

    if (openedFile)
    {
        isFileOpened_b = true;
    }
    else
    {
        isFileOpened_b = false;
    }
    return isFileOpened_b;
}

bool SDCardManager::HasData()
{
    return isFileOpened_b && openedFile.available();
}

bool SDCardManager::CreateFile(char *filePath)
{
    File file = SD.open(filePath, FILE_WRITE);
    if (file)
    {
        file.close();
        return true;
    }

    return false;
}

bool SDCardManager::CreateFolder(char *folderPath)
{
    if (!SD.exists(folderPath))
    {
        if (!SD.mkdir(folderPath))
        {
            return false;
        }
    }
    return true;
}

bool SDCardManager::ExistsFile(char *filePath)
{
    return SD.exists(filePath);
}

void SDCardManager::CloseFile()
{
    if (isFileOpened_b)
    {
        openedFile.flush();
        openedFile.close();
    }
    isFileOpened_b = false;
}

bool SDCardManager::WriteRawByte(char byte)
{
    if (!isFileOpened_b)
    {
        return false;
    }
    openedFile.write(byte);

    return true;
}

bool SDCardManager::WriteRawBytes(char *bytes, size_t size)
{
    if (!isFileOpened_b)
    {
        return false;
    }
    openedFile.write(bytes, size);

    return true;
}

bool SDCardManager::ReadRawBytes(char *bytes, size_t size)
{
    if (!isFileOpened_b)
    {
        return false;
    }

    openedFile.readBytes(bytes, size);

    return true;
}

void SDCardManager::CardInfo()
{
    Sd2Card card;
    SdVolume volume;
    SdFile root;

    if (!card.init(SPI_HALF_SPEED, PIN_SD_CARD_CS))
    {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card inserted?");
        Serial.println("* is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        while (1)
            ;
    }
    else
    {
        Serial.println("Wiring is correct and a card is present.");
    }
    Serial.println();
    Serial.print("Card type:         ");
    switch (card.type())
    {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
    default:
        Serial.println("Unknown");
    }

    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card))
    {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        while (1)
            ;
    }

    Serial.print("Clusters:          ");
    Serial.println(volume.clusterCount());
    Serial.print("Blocks x Cluster:  ");
    Serial.println(volume.blocksPerCluster());

    Serial.print("Total Blocks:      ");
    Serial.println(volume.blocksPerCluster() * volume.clusterCount());
    Serial.println();

    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("Volume type is:    FAT");
    Serial.println(volume.fatType(), DEC);

    volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
    volumesize *= volume.clusterCount();    // we'll have a lot of clusters
    volumesize /= 2;                        // SD card blocks are always 512 bytes (2 blocks are 1KB)
    Serial.print("Volume size (Kb):  ");
    Serial.println(volumesize);
    Serial.print("Volume size (Mb):  ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Gb):  ");
    Serial.println((float)volumesize / 1024.0);

    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
}

void SDCardManager::ListFiles()
{
    if (!isConnected_b)
    {
        Serial.println("Disconnected");
        return;
    }

    File root = SD.open("/");

    PrintDirectory(root, 0);

    root.close();
}

void SDCardManager::DumpFile(char *filePath)
{
    if (!isConnected_b)
    {
        return;
    }

    if (!SD.exists(filePath))
    {
        Serial.println("No File at specified path");
        return;
    }
    File dataFile = SD.open(filePath);
    if (dataFile)
    {
        while (dataFile.available())
        {
            Serial.write(dataFile.read());
        }
        dataFile.close();
    }
    else
    {
        Serial.println("Error at opening file!");
    }
}

void SDCardManager::PrintDirectory(File dir, int numTabs)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }

        Serial.print(entry.name());

        if (entry.isDirectory())
        {
            Serial.println("/");
            PrintDirectory(entry, numTabs + 1);
        }
        else
        {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}
