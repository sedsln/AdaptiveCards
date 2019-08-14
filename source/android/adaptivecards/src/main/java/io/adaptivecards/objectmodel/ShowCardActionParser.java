/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package io.adaptivecards.objectmodel;

public class ShowCardActionParser extends ActionElementParser {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected ShowCardActionParser(long cPtr, boolean cMemoryOwn) {
    super(AdaptiveCardObjectModelJNI.ShowCardActionParser_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ShowCardActionParser obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void swigSetCMemOwn(boolean own) {
    swigCMemOwnDerived = own;
    super.swigSetCMemOwn(own);
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwnDerived) {
        swigCMemOwnDerived = false;
        AdaptiveCardObjectModelJNI.delete_ShowCardActionParser(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public ShowCardActionParser() {
    this(AdaptiveCardObjectModelJNI.new_ShowCardActionParser__SWIG_0(), true);
  }

  public ShowCardActionParser(ShowCardActionParser arg0) {
    this(AdaptiveCardObjectModelJNI.new_ShowCardActionParser__SWIG_1(ShowCardActionParser.getCPtr(arg0), arg0), true);
  }

  public BaseActionElement Deserialize(ParseContext context, JsonValue value) {
    long cPtr = AdaptiveCardObjectModelJNI.ShowCardActionParser_Deserialize(swigCPtr, this, ParseContext.getCPtr(context), context, JsonValue.getCPtr(value), value);
    return (cPtr == 0) ? null : new BaseActionElement(cPtr, true);
  }

  public BaseActionElement DeserializeFromString(ParseContext context, String jsonString) {
    long cPtr = AdaptiveCardObjectModelJNI.ShowCardActionParser_DeserializeFromString(swigCPtr, this, ParseContext.getCPtr(context), context, jsonString);
    return (cPtr == 0) ? null : new BaseActionElement(cPtr, true);
  }

}
