#pragma once
#include <string>

namespace THPrac {
bool LauncherMktGuiUpd();
bool LauncherMktIsStreaming();
std::string& LauncherMktGetCtx();
void LauncherMktLaunchCallback(const char* game);
void LauncherMktTerminateAll();
}