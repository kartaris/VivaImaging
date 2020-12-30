#pragma once

#ifndef _MODULE_K_IMAGE_PROPERTY_HEADER_
#define _MODULE_K_IMAGE_PROPERTY_HEADER_

typedef enum class tagPropertyGroupMode
{
  PModeImage,
  PModeAnnotate,
  PModeBlockEdit
} PropertyGroupMode;

typedef enum class tagPropertyMode
{
  PModeImageView = 0,
  PModeRotate,
  PModeResize,
  PModeCrop,

  PModeEditImage,

  PModeMonochrome,
  PModeColorBalance,
  PModeColorLevel,
  PModeColorBright,
  PModeColorHLS,
  PModeColorHSV,
  PModeColorReverse,

  PModeAnnotateSelect,
  PModeAnnotateLine,
  PModeAnnotateFreehand,
  PModeAnnotateRectangle,
  PModeAnnotateEllipse,
  PModeAnnotatePolygon,
  PModeAnnotateTextbox,
  PModeAnnotateEditText,

  PModeAnnotateShape,
  PModeAnnotateShapeLine = PModeAnnotateShape,
  PModeAnnotateShapeLineEndArrow,
  PModeAnnotateShapeRectangle,
  PModeAnnotateShapeEllipse,
  PModeAnnotateShapeRoundRectangle,
  PModeAnnotateShapeTriangle,
  PModeAnnotateShapeRightAngleLine,
  PModeAnnotateShapeRightAngleLineEndArrow,
  //PModeAnnotatePolygon,
  //PModeAnnotateFreehand,
  PModeAnnotateRightArrow,
  PModeAnnotateBottomArrow,
  PModeAnnotateArc,
  PModeAnnotateSplineCurve,
  PModeAnnotateLeftBrace,
  PModeAnnotateRightBrace,

  PModeAnnotateShapeLast = PModeAnnotateShape+200,

  PModeSelectRectangle,
  PModeSelectTriangle,
  PModeSelectRhombus,
  PModeSelectPentagon,
  PModeSelectStar,
  PModeSelectEllipse,
  PModeSelectFill,
  PModeCopyRectangle,
  PModeCopyTriangle,
  PModeCopyRhombus,
  PModeCopyPentagon,
  PModeCopyStar,
  PModeCopyEllipse,
  PModePasteImage,

  PModeMax
}PropertyMode;

#define IsSelectShapeMode(m) ((PropertyMode::PModeSelectRectangle <= m) && (m <= PropertyMode::PModeSelectEllipse))
#define IsSelectBlockMode(m) ((PropertyMode::PModeSelectRectangle <= m) && (m <= PropertyMode::PModePasteImage))
#define IsAnnotateMode(m) ((PropertyMode::PModeAnnotateSelect <= m) && (m <= PropertyMode::PModeAnnotateShapeLast))
#define IsAnnotateShapeMode(m) ((PropertyMode::PModeAnnotateShape <= m) && (m <= PropertyMode::PModeAnnotateShapeLast))

typedef enum class tagRotateMode
{
  ROTATE_NONE,
  ROTATE_90CW,
  ROTATE_90CCW,
  ROTATE_180,
  FLIP_HORZ,
  FLIP_VERT
} RotateMode;


typedef enum class tagReverseMode
{
  REVERSE_NONE,
  REVERSE_RGB,
  REVERSE_VALUE,
  REVERSE_LIGHTNESS
} ReverseMode;

#define ImageEffectFillStyle 0x01
#define ImageEffectLineStyle 0x02
#define ImageEffectFontStyle 0x04

typedef struct tagImageEffectJob
{
  PropertyMode job;
  int channel;
  int param[8];
  int applyAll;
  LPVOID extra;
  LPVOID pResult;
} ImageEffectJob;

extern int CompareEffect(ImageEffectJob* d, ImageEffectJob* s);

#endif // _MODULE_K_IMAGE_PROPERTY_HEADER_
