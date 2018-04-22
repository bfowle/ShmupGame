#include "BarrageManager.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

void BarrageManager::loadBulletMLFiles() {
    FString sourceDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + "BulletML";
    FPaths::NormalizeDirectoryName(sourceDir);

    IFileManager &fileManager = IFileManager::Get();

    TArray<FString> files;
    fileManager.FindFilesRecursive(files, *(sourceDir), TEXT("*.xml"), true, false);
    for (int i = 0; i < files.Num(); ++i) {
        //UE_LOG(LogTemp, Warning, TEXT(" parsing: %s"), *files[i]);
        BulletMLParserTinyXML *parser = new BulletMLParserTinyXML(const_cast<char *>(TCHAR_TO_UTF8(*files[i])));
        parser->parse();
        m_parser.push_back(parser);
    }
}

void BarrageManager::unloadBulletMLFiles() {
    for (int i = 0; i < m_parser.size(); ++i) {
        delete m_parser[i];
        m_parser[i] = nullptr;
    }
}
