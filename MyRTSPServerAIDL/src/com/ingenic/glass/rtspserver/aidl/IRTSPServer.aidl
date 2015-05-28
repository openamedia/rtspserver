package com.ingenic.glass.rtspserver.aidl;

import com.ingenic.glass.rtspserver.aidl.IRTSPServerListener;

interface IRTSPServer {
    void start();
    void stop();
    void writeLiveH264(in byte[] buf, int offset, int length, int flag);
    
    void setListener(IRTSPServerListener listener);
}