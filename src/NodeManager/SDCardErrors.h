#ifndef SD_CARD_ERROS_H
#define SD_CARD_ERROS_H

enum SDCardErrors : char
{
    SD_INIT_ERROR = 0,
    SD_OPEN_FILE_ERROR,
    SD_CREATE_FILE_ERROR,
    SD_CREATE_FOLDER_ERROR,
    SD_READ_ERROR,
    SD_WRITE_ERROR
};

#endif // SD_CARD_ERROS_H