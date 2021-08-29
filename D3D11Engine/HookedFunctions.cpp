#include "pch.h"
#include "HookedFunctions.h"

#include "zCBspTree.h"
#include "zCWorld.h"
#include "oCGame.h"
#include "zCMaterial.h"
#include "zFILE.h"
#include "zCOption.h"
#include "zCRndD3D.h"
#include "zCParticleFX.h"
#include "zCView.h"
#include "CGameManager.h"
#include "zCVisual.h"
#include "zCTimer.h"
#include "zCModel.h"
#include "oCSpawnManager.h"
#include "zCVob.h"
#include "zCTexture.h"
#include "zCThread.h"
#include "zCResourceManager.h"
#include "zCQuadMark.h"
#include "oCNPC.h"
#include "zCSkyController_Outdoor.h"

#include "zQuat.h"
#include "zMat4.h"

#if _MSC_VER >= 1300
#include <Tlhelp32.h>
#endif

#include "StackWalker.h"

/** Init all hooks here */
void HookedFunctionInfo::InitHooks() {
    LogInfo() << "Initializing hooks";

    DWORD dwProtect;
    VirtualProtect( (void*)GothicMemoryLocations::zCWorld::Render, 0x255, PAGE_EXECUTE_READWRITE, &dwProtect );

    oCGame::Hook();
    zCBspTree::Hook();
    zCWorld::Hook();
    zCMaterial::Hook();
    zCBspNode::Hook();
    zFILE::Hook();
    zCOption::Hook();
    zCRndD3D::Hook();
    zCParticleFX::Hook();
    zCView::Hook();
    CGameManager::Hook();
    zCVisual::Hook();
    zCTimer::Hook();
    zCModel::Hook();
    zCModelPrototype::Hook();
    oCSpawnManager::Hook();
    zCVob::Hook();
    zCTexture::Hook();
    zCThread::Hook();
    //zCResourceManager::Hook();
    zCQuadMark::Hook();
    oCNPC::Hook();
    zCSkyController_Outdoor::Hook();

    //XHook(original_zCExceptionHandler_UnhandledExceptionFilter, GothicMemoryLocations::Functions::zCExceptionHandler_UnhandledExceptionFilter, HookedFunctionInfo::hooked_zCExceptionHandlerUnhandledExceptionFilter);
    //XHook(original_HandledWinMain, GothicMemoryLocations::Functions::HandledWinMain, HookedFunctionInfo::hooked_HandledWinMain);
    //XHook(original_ExitGameFunc, GothicMemoryLocations::Functions::ExitGameFunc, HookedFunctionInfo::hooked_ExitGameFunc);

    // Kill the check for doing freelook only in fullscreen, since we force the game to run windowed internally
    //int flSize = GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckEnd - GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart;
    //VirtualProtect((void *)GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart, flSize, PAGE_EXECUTE_READWRITE, &dwProtect);
    //REPLACE_RANGE(GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart, GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckEnd-1, INST_NOP);

    // Hook the single bink-function
    XHook( GothicMemoryLocations::zCBinkPlayer::GetPixelFormat, HookedFunctionInfo::hooked_zBinkPlayerGetPixelFormat );

#if defined(BUILD_GOTHIC_2_6_fix) || defined(BUILD_GOTHIC_1_08k)
    XHook( original_zCBinkPlayerOpenVideo, GothicMemoryLocations::zCBinkPlayer::OpenVideo, HookedFunctionInfo::hooked_zBinkPlayerOpenVideo );
#endif
    original_Alg_Rotation3DNRad = (Alg_Rotation3DNRad)GothicMemoryLocations::Functions::Alg_Rotation3DNRad;

//G1 patches
#ifdef BUILD_GOTHIC_1_08k
#ifdef BUILD_1_12F
    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x00506B31, "\xEB" );
#else
    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x004F4024, "\xEB" );
    PatchAddr( 0x004F43FC, "\xEB" );
#endif
#endif

//G2 patches
#ifdef BUILD_GOTHIC_2_6_fix
    zQuat::Hook();
    zMat4::Hook();

    // Workaround to fix disappearing ui elements under certain circumstances
    // e.g. praying at Beliar statue, screen blend causing dialog boxes to disappear.
    LogInfo() << "Patching: Overriding zCVobScreenFX::OnTick() if (blend.visible) -> if (false)";
    PatchAddr( 0x61808Fu, "\x90\xE9" );

    LogInfo() << "Patching: Interupt gamestart sound";
    PatchAddr( 0x004DB89F, "\x00" );

    LogInfo() << "Patching: Fix low framerate";
    PatchAddr( 0x004DDC6F, "\x08" );

    LogInfo() << "Patching: LOW_FPS_NaN_check";
    PatchAddr( 0x0066E59A, "\x81\x3A\x00\x00\xC0\xFF\x0F\x84\xF3\x3C\xEC\xFF\x81\x3A\x00\x00\xC0\x7F\x0F\x84\xE7\x3C\xEC\xFF\xD9\x45\x00\x8D\x44\x8C\x20\xE9\xEB\x3C\xEC\xFF" );
    PatchAddr( 0x005322A2, "\xE9\xF3\xC2\x13\x00\x90\x90" );
    PatchAddr( 0x0066E5BE, "\x81\x7C\xE4\x20\x00\x00\xC0\xFF\x0F\x84\x2A\x2B\xEC\xFF\x81\x7C\xE4\x20\x00\x00\xC0\x7F\x0F\x84\x1C\x2B\xEC\xFF\xE9\xC1\x2A\xEC\xFF" );
    PatchAddr( 0x0061E412, "\xE8\xA7\x01\x05\x00" );

    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x00502F94, "\xEB" );
    PatchAddr( 0x00503343, "\xEB" );

#endif
    // HACK Workaround to fix debuglines in godmode
#if (defined BUILD_GOTHIC_2_6_fix)
    LogInfo() << "Patching: Godmode Debuglines";
    // oCMagFrontier::GetDistanceNewWorld
    PatchAddr( 0x00473f37, "\xBD\x00\x00\x00\x00" ); // replace MOV EBP, 0x1 with MOV EBP, 0x0
    // oCMagFrontier::GetDistanceDragonIsland
    PatchAddr( 0x004744c1, "\xBF\x00\x00\x00\x00" ); // replace MOV EDI, 0x1 with MOV EDI, 0x0
    // oCMagFrontier::GetDistanceAddonWorld
    PatchAddr( 0x00474681, "\xBF\x00\x00\x00\x00" ); // replace MOV EDI, 0x1 with MOV EDI, 0x0
#endif
}

/** Function hooks */
int __stdcall HookedFunctionInfo::hooked_HandledWinMain( HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int sw ) {
    int r = HookedFunctions::OriginalFunctions.original_HandledWinMain( hInstance, hPrev, szCmdLine, sw );

    return r;
}

void __fastcall HookedFunctionInfo::hooked_zCActiveSndAutoCalcObstruction( void* thisptr, void* unknwn, int i ) {
    // Just do nothing here. Something was inside zCBspTree::Render that managed this and thus voices get really quiet in indoor locations
    // This function is for calculating the automatic volume-changes when the camera goes in/out buildings
    // We keep everything on the same level by removing it
}

void __cdecl HookedFunctionInfo::hooked_ExitGameFunc() {
    Engine::OnShutDown();

    HookedFunctions::OriginalFunctions.hooked_ExitGameFunc();
}

long __stdcall HookedFunctionInfo::hooked_zCExceptionHandlerUnhandledExceptionFilter( EXCEPTION_POINTERS* exceptionPtrs ) {
    return HookedFunctions::OriginalFunctions.original_zCExceptionHandler_UnhandledExceptionFilter( exceptionPtrs );
}

/** Returns the pixelformat of a bink-surface */
long __fastcall HookedFunctionInfo::hooked_zBinkPlayerGetPixelFormat( void* thisptr, void* unknwn, zTRndSurfaceDesc& desc ) {
    int* cd = (int*)&desc;

    // Resolution is at pos [2] and [3]
    //cd[2] = Engine::GraphicsEngine->GetResolution().x;
    //cd[3] = Engine::GraphicsEngine->GetResolution().y;

    /*for(int i=0;i<0x7C;i++)
    {
    cd[i] = i;
    }*/

    return 4; // 4 satisfies gothic enough to play the video
    //Global::HookedFunctions.zBinkPlayerGetPixelFormat(thisptr, desc);
}

// Using zSTRING directly here crashes because the class can't make proper copies but we don't need it anyway so let's do it the safe way
int __fastcall HookedFunctionInfo::hooked_zBinkPlayerOpenVideo( void* thisptr, void* unknwn, int a1, int a2, int a3, int a4, int a5 ) {
    int r = HookedFunctions::OriginalFunctions.original_zCBinkPlayerOpenVideo( thisptr, a1, a2, a3, a4, a5 );

    struct BinkInfo {
        unsigned int ResX;
        unsigned int ResY;
        // ... unimportant
    };

    // Grab the resolution
    // This structure stores width and height as first two parameters, as ints.
    BinkInfo* res = *(BinkInfo**)(((char*)thisptr) + (GothicMemoryLocations::zCBinkPlayer::Offset_VideoHandle));

    if ( res ) {
        Engine::GAPI->GetRendererState().RendererInfo.PlayingMovieResolution = INT2( res->ResX, res->ResY );
    }

    return r;
}
