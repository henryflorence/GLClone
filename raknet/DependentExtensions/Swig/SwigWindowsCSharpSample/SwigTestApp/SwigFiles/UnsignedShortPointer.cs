/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace RakNet {

using System;
using System.Runtime.InteropServices;

public class UnsignedShortPointer : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal UnsignedShortPointer(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(UnsignedShortPointer obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~UnsignedShortPointer() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          RakNetPINVOKE.delete_UnsignedShortPointer(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public UnsignedShortPointer() : this(RakNetPINVOKE.new_UnsignedShortPointer(), true) {
  }

  public void assign(ushort value) {
    RakNetPINVOKE.UnsignedShortPointer_assign(swigCPtr, value);
  }

  public ushort value() {
    ushort ret = RakNetPINVOKE.UnsignedShortPointer_value(swigCPtr);
    return ret;
  }

  public SWIGTYPE_p_unsigned_short cast() {
    IntPtr cPtr = RakNetPINVOKE.UnsignedShortPointer_cast(swigCPtr);
    SWIGTYPE_p_unsigned_short ret = (cPtr == IntPtr.Zero) ? null : new SWIGTYPE_p_unsigned_short(cPtr, false);
    return ret;
  }

  public static UnsignedShortPointer frompointer(SWIGTYPE_p_unsigned_short t) {
    IntPtr cPtr = RakNetPINVOKE.UnsignedShortPointer_frompointer(SWIGTYPE_p_unsigned_short.getCPtr(t));
    UnsignedShortPointer ret = (cPtr == IntPtr.Zero) ? null : new UnsignedShortPointer(cPtr, false);
    return ret;
  }

}

}
