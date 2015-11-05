#ifndef BONES_CORE_H_
#define BONES_CORE_H_


#include <stdint.h>
#include <memory.h>
#include <assert.h>

#include <Windows.h>


namespace bones
{
extern const char * kClassRoot;
extern const char * kClassView;
extern const char * kClassImage;
extern const char * kClassShape;
extern const char * kClassText;
extern const char * kClassInput;
extern const char * kClassRichEdit;
extern const char * kClassWebView;
extern const char * kClassScroller;

typedef float Scalar;
typedef HANDLE Cursor;
typedef HBITMAP Caret;
typedef HWND Widget;

}
#endif