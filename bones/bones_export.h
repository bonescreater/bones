#ifndef BONES_BONES_EXPORT_H_
#define BONES_BONES_EXPORT_H_

#include "bones.h"

BONES_API_EXPORT(bool) BonesStartUp(const bones::Core::Config & config);

BONES_API_EXPORT(void) BonesShutDown();

BONES_API_EXPORT(void) BonesUpdate();

BONES_API_EXPORT(const wchar_t *) BonesGetVersion();

BONES_API_EXPORT(bool) BonesLoadXMLString(const char * data);

BONES_API_EXPORT(void) BonesCleanXML();

BONES_API_EXPORT(bones::Pixmap *)BonesDecodePixmap(const void * data, int len);

BONES_API_EXPORT(void)BonesFreePixmap(bones::Pixmap * pm);

BONES_API_EXPORT(bones::Ref *) BonesGetCObjectByID(const char * id);

BONES_API_EXPORT(void) BonesRegScriptCallback(bones::Ref * co,
                                              const char * event_name,
                                              bones::ScriptCallBack cb,
                                              void * userdata);

BONES_API_EXPORT(void) BonesUnregScriptCallback(bones::Ref * co,
                                                const char * event_name);

BONES_API_EXPORT(void)BonesApplyCSS(bones::Ref * co, const char * css);

BONES_API_EXPORT(void)BonesApplyClass(bones::Ref * co, const char * class_name);


#endif