#pragma once

// a way to check all available default icons on current system

#if 0
std::array ard_id_array = {
wxART_ADD_BOOKMARK         ,
wxART_DEL_BOOKMARK         ,
wxART_HELP_SIDE_PANEL      ,
wxART_HELP_SETTINGS        ,
wxART_HELP_PAGE            ,
wxART_GO_BACK              ,
wxART_GO_FORWARD           ,
wxART_GO_UP                ,
wxART_GO_DOWN              ,
wxART_GO_TO_PARENT         ,
wxART_GO_HOME              ,
wxART_GOTO_FIRST           ,
wxART_GOTO_LAST            ,
wxART_FILE_OPEN            ,
wxART_FILE_SAVE            ,
wxART_FILE_SAVE_AS         ,
wxART_PRINT                ,
wxART_HELP                 ,
wxART_TIP                  ,
wxART_REPORT_VIEW          ,
wxART_LIST_VIEW            ,
wxART_NEW_DIR              ,
wxART_HARDDISK             ,
wxART_FLOPPY               ,
wxART_CDROM                ,
wxART_REMOVABLE            ,
wxART_FOLDER               ,
wxART_FOLDER_OPEN          ,
wxART_GO_DIR_UP            ,
wxART_EXECUTABLE_FILE      ,
wxART_NORMAL_FILE          ,
wxART_TICK_MARK            ,
wxART_CROSS_MARK           ,
wxART_ERROR                ,
wxART_QUESTION             ,
wxART_WARNING              ,
wxART_INFORMATION          ,
wxART_MISSING_IMAGE        ,
wxART_COPY                 ,
wxART_CUT                  ,
wxART_PASTE                ,
wxART_DELETE               ,
wxART_NEW                  ,
wxART_PLUS                 ,
wxART_MINUS                ,
wxART_CLOSE                ,
wxART_QUIT                 ,
wxART_FIND                 ,
wxART_FIND_AND_REPLACE     ,
wxART_FULL_SCREEN          ,
wxART_EDIT                 ,
wxART_WX_LOGO              ,
wxART_REFRESH              ,
wxART_STOP };

auto test_menu = new wxMenu();
int id_base = 9900;
for (size_t i = 0; i < ard_id_array.size(); i++)
{
    auto some_menu = test_menu->Append(id_base, std::format("menu item {}", i));
    some_menu->SetBitmap(wxArtProvider::GetBitmap(ard_id_array[i], wxART_MENU));
    id_base++;
}
menu_bar->Append(test_menu, "Test");
#endif
