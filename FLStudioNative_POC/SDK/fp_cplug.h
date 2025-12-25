/*==============================================================================
    FL Studio SDK - C++ Plugin Helper Class
    Minimal version for POC purposes
==============================================================================*/

#pragma once

#include "fp_plugclass.h"
#include <cstring>

// C++ helper class that implements common functionality
class TCPPFruityPlug : public TFruityPlug
{
protected:
    int Tag;                          // Plugin instance tag
    TFruityPlugHost* Host;            // Host callback structure
    TFruityPlugInfo Info;             // Plugin info
    char LongNameBuffer[256];         // Buffer for long name
    char ShortNameBuffer[64];         // Buffer for short name
    
public:
    TCPPFruityPlug(int PlugTag, TFruityPlugHost* PlugHost)
        : Tag(PlugTag)
        , Host(PlugHost)
    {
        // Initialize info structure
        std::memset(&Info, 0, sizeof(Info));
        std::memset(LongNameBuffer, 0, sizeof(LongNameBuffer));
        std::memset(ShortNameBuffer, 0, sizeof(ShortNameBuffer));
        
        Info.SDKVersion = 1;
        Info.LongName = LongNameBuffer;
        Info.ShortName = ShortNameBuffer;
    }
    
    virtual ~TCPPFruityPlug() override {}
    
    // Default implementation - derived class should override
    void DestroyObject() override
    {
        delete this;
    }
    
    // Helper function to call host dispatcher
    template<typename T>
    T* HostDispatcher(int Index, int Value, void* Ptr)
    {
        if (Host && Host->Dispatcher)
            return static_cast<T*>(Host->Dispatcher((void*)this, Index, Value, Ptr));
        return nullptr;
    }
    
    // Helper to notify host of parameter change
    void NotifyHostParamChanged(int Index, int Value)
    {
        if (Host && Host->OnParamChanged)
            Host->OnParamChanged((void*)this, Index, Value);
    }
    
    // Helper to show hint in host
    void ShowHint(int Index, const char* Hint)
    {
        if (Host && Host->OnHint)
            Host->OnHint((void*)this, Index, Hint);
    }
};
