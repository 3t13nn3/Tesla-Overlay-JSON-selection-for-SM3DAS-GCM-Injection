#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "dir_iterator.hpp"
constexpr const char *const JSONContentsPath = "/JSON_ROM";
constexpr const char *TargetedPath = "/atmosphere/contents/010049900F546002/romfs"; // atmosphere/contents/010049900F546002/romfs/rom.json

/*Some functions picked from here: https://github.com/joel16/NX-Shell/blob/972e25ca41ef5a363d9e656002090c140a1ebeed/source/fs.cpp */
//To correct
/*void createDirDeep(FsFileSystem * fs, std::string path){
    int cpt = 0;
    std::string deepestDir = "";
    for(const char& c : path){
        
        if(c == '/'){
            cpt++;
            if(cpt != 1){
                bool dirExist = false;
                FsDir dir;
                Result res = fsFsOpenDirectory(fs, TargetedPath, FsDirOpenMode_ReadDirs, &dir);
                if (R_SUCCEEDED(res)) {
                    fsDirClose(&dir);
                    dirExist = true;
                }
                if(!dirExist){
                    fsFsCreateDirectory(fs, deepestDir.c_str());
                }
            }
        }
        deepestDir+=c;
    }
    fsFsCreateDirectory(fs, deepestDir.c_str());  
}*/

static Result CopyFile(const char srcPath[FS_MAX_PATH], const char destPath[FS_MAX_PATH])
{

    FsFile src_handle, dest_handle;
    FsFileSystem fs;

    fsOpenSdCardFileSystem(&fs);

    Result res = fsFsOpenFile(&fs, srcPath, FsOpenMode_Read, &src_handle);
    if (R_FAILED(res))
    {
        return false;
    }

    s64 size = 0;
    res = fsFileGetSize(&src_handle, &size);
    if (R_FAILED(res))
    {
        fsFileClose(&src_handle);
        return false;
    }

    bool fileExist = false;
    bool dirExist = false;
    FsFile file;

    FsDir dir;
    res = fsFsOpenDirectory(&fs, TargetedPath, FsDirOpenMode_ReadDirs, &dir);
    if (R_SUCCEEDED(res))
    {
        fsDirClose(&dir);
        dirExist = true;
    }

    if (dirExist)
        fsFsCreateDirectory(&fs, TargetedPath);

    res = fsFsOpenFile(&fs, destPath, FsOpenMode_Read, &file);
    if (R_SUCCEEDED(res))
    {
        fsFileClose(&file);
        fileExist = true;
    }

    if (!fileExist)
        fsFsCreateFile(&fs, destPath, size, 0);

    res = fsFsOpenFile(&fs, destPath, FsOpenMode_Write, &dest_handle);
    if (R_FAILED(res))
    {
        fsFileClose(&src_handle);
        return false;
    }

    u64 bytes_read = 0;
    const u64 buf_size = 0x10000;
    s64 offset = 0;
    unsigned char *buf = new unsigned char[buf_size];

    do
    {
        std::memset(buf, 0, buf_size);

        res = fsFileRead(&src_handle, offset, buf, buf_size, FsReadOption_None, &bytes_read);
        if (R_FAILED(res))
        {
            delete[] buf;
            fsFileClose(&src_handle);
            fsFileClose(&dest_handle);
            return false;
        }

        res = fsFileWrite(&dest_handle, offset, buf, bytes_read, FsWriteOption_Flush);
        if (R_FAILED(res))
        {
            delete[] buf;
            fsFileClose(&src_handle);
            fsFileClose(&dest_handle);
            return false;
        }

        offset += bytes_read;
    } while (offset < size);

    delete[] buf;
    fsFileClose(&src_handle);
    fsFileClose(&dest_handle);

    return res;
}

Result Delete(std::string path) {

        FsFileSystem fs;
        fsOpenSdCardFileSystem(&fs);

		Result res = fsFsDeleteFile(&fs, path.c_str());

		return res;
	}

class GuiSecondary : public tsl::Gui
{
public:
    GuiSecondary() {}

    virtual tsl::elm::Element *createUI() override
    {
        auto *rootFrame = new tsl::elm::OverlayFrame("ROM Injection", "v0.1 - Secondary Gui");

        rootFrame->setContent(new tsl::elm::DebugRectangle(tsl::Color{0x8, 0x3, 0x8, 0xF}));

        return rootFrame;
    }
};

class GuiTest : public tsl::Gui
{
public:
    GuiTest(u8 arg1, u8 arg2, bool arg3) {}

    virtual tsl::elm::Element *createUI() override
    {

        auto frame = new tsl::elm::OverlayFrame("ROM Injection", "v0.1");
        
        auto list = new tsl::elm::List();
        

        static char pathBuffer[FS_MAX_PATH];
        FsFileSystem fs;

        fsOpenSdCardFileSystem(&fs);

        FsDir contentDir;
        std::strcpy(pathBuffer, JSONContentsPath);

        fsFsOpenDirectory(&fs, pathBuffer, FsDirOpenMode_ReadFiles, &contentDir);

        tsl::hlp::ScopeGuard dirGuard([&] { fsDirClose(&contentDir); });

        fsFsCreateDirectory(&fs, "/atmosphere/contents/");
        fsFsCreateDirectory(&fs, "/atmosphere/contents/010049900F546002");
        fsFsCreateDirectory(&fs, "/atmosphere/contents/010049900F546002/romfs");

        list->addItem(new tsl::elm::CategoryHeader("Delete custom JSON"));

        auto *clickableListItem = new tsl::elm::ListItem("Delete \"" + std::string(TargetedPath) + "/rom.json\"", "");
        clickableListItem->setClickListener([](u64 keys) {
            if (keys & KEY_A)
            {

                Delete(std::string(TargetedPath) + "/rom.json");

                return true;
            }

            return false;
        });

        list->addItem(clickableListItem);

        list->addItem(new tsl::elm::CategoryHeader("JSON files selection"));

        for (const auto &entry : FsDirIterator(contentDir))
        {
            std::string fileName(entry.name);
            if (fileName[fileName.length() - 5] == '.' && ((fileName[fileName.length() - 4] == 'j' && fileName[fileName.length() - 3] == 's' && fileName[fileName.length() - 2] == 'o' && fileName[fileName.length() - 1] == 'n') || (fileName[fileName.length() - 4] == 'J' && fileName[fileName.length() - 3] == 'S' && fileName[fileName.length() - 2] == 'O' && fileName[fileName.length() - 1] == 'N')))
            {
                auto *clickableListItem = new tsl::elm::ListItem(fileName, "");
                clickableListItem->setClickListener([fileName](u64 keys) {
                    if (keys & KEY_A)
                    {

                        std::string srcPath(JSONContentsPath);
                        srcPath += "/" + fileName;
                        std::string destPath(TargetedPath);
                        destPath += "/rom.json";

                        CopyFile(srcPath.c_str(), destPath.c_str());

                        return true;
                    }

                    return false;
                });

                list->addItem(clickableListItem);
            }
        }
        fsFsClose(&fs);

        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override
    {
    }

    // Called once every frame to handle inputs not handled by other UI elements
    virtual bool handleInput(u64 keysDown, u64 keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override
    {
        return false; // Return true here to signal the inputs have been consumed
    }
};

class OverlayTest : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {} // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {} // Called at the end to clean up all services previously initialized

    virtual void onShow() override {} // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {} // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiTest>(1, 2, true); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayTest>(argc, argv);
}