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

public class RakNetPageRow : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal RakNetPageRow(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(RakNetPageRow obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~RakNetPageRow() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          RakNetPINVOKE.delete_RakNetPageRow(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public bool isLeaf {
    set {
      RakNetPINVOKE.RakNetPageRow_isLeaf_set(swigCPtr, value);
    } 
    get {
      bool ret = RakNetPINVOKE.RakNetPageRow_isLeaf_get(swigCPtr);
      return ret;
    } 
  }

  public int size {
    set {
      RakNetPINVOKE.RakNetPageRow_size_set(swigCPtr, value);
    } 
    get {
      int ret = RakNetPINVOKE.RakNetPageRow_size_get(swigCPtr);
      return ret;
    } 
  }

  public RakNetPageRow next {
    set {
      RakNetPINVOKE.RakNetPageRow_next_set(swigCPtr, RakNetPageRow.getCPtr(value));
    } 
    get {
      IntPtr cPtr = RakNetPINVOKE.RakNetPageRow_next_get(swigCPtr);
      RakNetPageRow ret = (cPtr == IntPtr.Zero) ? null : new RakNetPageRow(cPtr, false);
      return ret;
    } 
  }

  public RakNetPageRow previous {
    set {
      RakNetPINVOKE.RakNetPageRow_previous_set(swigCPtr, RakNetPageRow.getCPtr(value));
    } 
    get {
      IntPtr cPtr = RakNetPINVOKE.RakNetPageRow_previous_get(swigCPtr);
      RakNetPageRow ret = (cPtr == IntPtr.Zero) ? null : new RakNetPageRow(cPtr, false);
      return ret;
    } 
  }

  public RakNetPageRow() : this(RakNetPINVOKE.new_RakNetPageRow(), true) {
  }

}

}
