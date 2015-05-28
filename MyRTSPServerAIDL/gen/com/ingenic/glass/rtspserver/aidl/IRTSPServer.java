/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/jqian/srcs/MyRTSPServerAIDL/src/com/ingenic/glass/rtspserver/aidl/IRTSPServer.aidl
 */
package com.ingenic.glass.rtspserver.aidl;
public interface IRTSPServer extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.ingenic.glass.rtspserver.aidl.IRTSPServer
{
private static final java.lang.String DESCRIPTOR = "com.ingenic.glass.rtspserver.aidl.IRTSPServer";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.ingenic.glass.rtspserver.aidl.IRTSPServer interface,
 * generating a proxy if needed.
 */
public static com.ingenic.glass.rtspserver.aidl.IRTSPServer asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.ingenic.glass.rtspserver.aidl.IRTSPServer))) {
return ((com.ingenic.glass.rtspserver.aidl.IRTSPServer)iin);
}
return new com.ingenic.glass.rtspserver.aidl.IRTSPServer.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_start:
{
data.enforceInterface(DESCRIPTOR);
this.start();
reply.writeNoException();
return true;
}
case TRANSACTION_stop:
{
data.enforceInterface(DESCRIPTOR);
this.stop();
reply.writeNoException();
return true;
}
case TRANSACTION_writeLiveH264:
{
data.enforceInterface(DESCRIPTOR);
byte[] _arg0;
_arg0 = data.createByteArray();
int _arg1;
_arg1 = data.readInt();
int _arg2;
_arg2 = data.readInt();
int _arg3;
_arg3 = data.readInt();
this.writeLiveH264(_arg0, _arg1, _arg2, _arg3);
reply.writeNoException();
return true;
}
case TRANSACTION_setListener:
{
data.enforceInterface(DESCRIPTOR);
com.ingenic.glass.rtspserver.aidl.IRTSPServerListener _arg0;
_arg0 = com.ingenic.glass.rtspserver.aidl.IRTSPServerListener.Stub.asInterface(data.readStrongBinder());
this.setListener(_arg0);
reply.writeNoException();
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.ingenic.glass.rtspserver.aidl.IRTSPServer
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public void start() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_start, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void stop() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_stop, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void writeLiveH264(byte[] buf, int offset, int length, int flag) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeByteArray(buf);
_data.writeInt(offset);
_data.writeInt(length);
_data.writeInt(flag);
mRemote.transact(Stub.TRANSACTION_writeLiveH264, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void setListener(com.ingenic.glass.rtspserver.aidl.IRTSPServerListener listener) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeStrongBinder((((listener!=null))?(listener.asBinder()):(null)));
mRemote.transact(Stub.TRANSACTION_setListener, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
}
static final int TRANSACTION_start = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_stop = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_writeLiveH264 = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_setListener = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
}
public void start() throws android.os.RemoteException;
public void stop() throws android.os.RemoteException;
public void writeLiveH264(byte[] buf, int offset, int length, int flag) throws android.os.RemoteException;
public void setListener(com.ingenic.glass.rtspserver.aidl.IRTSPServerListener listener) throws android.os.RemoteException;
}
