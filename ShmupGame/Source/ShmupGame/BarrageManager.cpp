#include "BarrageManager.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

array<const string, BarrageManager::BARRAGE_TYPE> BarrageManager::m_dirName = {
    "morph", "small", "small_move", "small_side_move",
    "medium", "medium_sub", "medium_move", "medium_back_move",
    "large", "large_move",
    "morph_lock", "small_lock", "medium_sub_lock"
};

void BarrageManager::loadBulletMLFiles() {
    FString sourceDir = FPaths::ProjectContentDir() + "BulletML/";
    FPaths::NormalizeDirectoryName(sourceDir);

    IFileManager &fileManager = IFileManager::Get();

    for (int i = 0; i < BARRAGE_TYPE; ++i) {
        TArray<FString> files;
        FString dirPath = sourceDir + "/" + m_dirName[i].c_str();
        fileManager.FindFiles(files, *(dirPath + "/*.xml"), true, false);
        int j = 0;
        for (; j < files.Num(); ++j) {
            FString filePath = dirPath + "/" + files[j];
            //UE_LOG(LogTemp, Warning, TEXT(" parsing: __ %s"), *filePath);
            m_parser[i][j] = BulletMLParserTinyXML_new(const_cast<char *>(TCHAR_TO_UTF8(*filePath)));
            BulletMLParserTinyXML_parse(m_parser[i][j]);
        }
        m_parserSizes[i] = j;
    }
}

void BarrageManager::unloadBulletMLFiles() {
    for (int i = 0; i < BARRAGE_TYPE; ++i) {
        for (int j = 0; j < m_parserSizes[i]; ++j) {
            delete m_parser[i][j];
            m_parser[i][j] = nullptr;
        }
    }
}
